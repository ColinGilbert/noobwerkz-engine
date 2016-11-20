#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"
#include "AudioInterfaceDroid.hpp"

uint32_t window_width;
uint32_t window_height;

size_t sample_rate;

EGLint current_context; 

std::string archive_dir;
std::unique_ptr<noob::application> app = nullptr;
std::atomic<bool> started(false);

extern "C"
{
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnShutdown(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint iWidth, jint iHeight);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnTouch(JNIEnv* env, jobject obj, int pointerID, float x, float y, int action);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnPause(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResume(JNIEnv* env, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_SetupArchiveDir(JNIEnv* env, jobject obj, jstring dir);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_CreateBufferQueueAudioPlayer(JNIEnv* env, jobject obj, int sampleRate, int bufSize);
};


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnInit()");

	if (!app)
	{
		app = std::unique_ptr<noob::application>(new noob::application());
	}
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnShutdown(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnShutdown()");

	opensl_wrapper_shutdown();//noob::audio_interface_droid::destroy();

	if (app)
	{
		// delete app;
		app = nullptr;
	}
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint width, jint height)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnResize()");

	window_height = height;
	window_width = width;

	EGLint last_context = current_context;

	current_context = reinterpret_cast<EGLint>(eglGetCurrentContext());

	if (current_context != last_context)
	{
		noob::logger::log(noob::importance::INFO, "[EngineDroid] New EGL context created.");
		if (last_context == 0)
		{
			noob::logger::log(noob::importance::INFO, "[EngineDroid] Initializing app.");
			app->init(window_width, window_height, archive_dir);
		}
	}

	app->window_resize(window_width, window_height);
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	if (app)
	{
		noob::logger::log(noob::importance::INFO, "[C++] Drawing frame");
		app->step();
	}
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnTouch(JNIEnv* env, jobject obj, int pointerID, float x, float y, int action)
{
	noob::logger::log(noob::importance::INFO, noob::concat("[C++] JNILib.OnTouch(", noob::to_string(x), ", ", noob::to_string(y), ")"));

	if (app)
	{
		app->touch(pointerID, x, y, action);
	}
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnPause(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnPause()");
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResume(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnResume()");

	if(app)
	{
		app->resume();
	}
}


std::string ConvertJString(JNIEnv* env, jstring str)
{
	if (!str)
	{
		return std::string();
	}

	const jsize len = env->GetStringUTFLength(str);
	const char* str_raw = env->GetStringUTFChars(str,(jboolean*)0);
	std::string result(str_raw, len);
	env->ReleaseStringUTFChars(str, str_raw);
	return result;
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_SetupArchiveDir(JNIEnv * env, jobject obj, jstring dir)
{
	const char* temp = env->GetStringUTFChars(dir, NULL);
	archive_dir = std::string(temp);

	noob::logger::log(noob::importance::INFO, noob::concat("JNILib.SetupArchiveDir(", archive_dir, ")"));

	if (app)
	{
		app->set_archive_dir(archive_dir);
	}

}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);

	noob::logger::log(noob::importance::INFO, temp);
}


int audio_cb(short* buffer, int num_samples)
{
	noob::globals& g = noob::globals::get_instance();

	g.master_mixer.tick(num_samples);
	// std::string s;
	uint32_t counter = 0;
	for (int frame = 0; frame < num_samples; ++frame)
	{
		double sample = g.master_mixer.output_buffer[frame];

		short converted = static_cast<short>(sample * 32767.0);
		// s += noob::concat(noob::to_string(sample), ",", noob::to_string(converted), " ");

		buffer[counter] = converted;
		buffer[counter+1] = converted;
		counter += 2;
	}
	

	// noob::logger::log(noob::importance::INFO, noob::concat("[AudioCallback] Wrote a buffer of ", noob::to_string(num_samples), " samples: ", s));
	return num_samples;
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_CreateBufferQueueAudioPlayer(JNIEnv* env, jobject obj, int sampleRate, int bufSize)
{
	noob::globals& g = noob::globals::get_instance();
	g.sample_rate = sampleRate;
	noob::logger::log(noob::importance::INFO, noob::concat("[C++] Created buffer queue player with samplerate ", noob::to_string(sampleRate), " and buffer size ", noob::to_string(bufSize)));
	sample_rate = sampleRate;
	opensl_wrapper_init(audio_cb, bufSize, sampleRate);
}
