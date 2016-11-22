#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

#include <thread>

#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"
#include "AudioInterfaceDroid.hpp"

namespace noob
{
	template<typename T> T idiv_ceil(T numerator, T denominator)
	{
		return numerator / denominator + (((numerator < 0) ^ (denominator > 0)) && (numerator % denominator));
	}
}

uint32_t window_width;
uint32_t window_height;


EGLint current_context; 

std::string archive_dir;
std::unique_ptr<noob::application> app = nullptr;
std::atomic<bool> started(false);
std::atomic<bool> playing_audio(false);
noob::ringbuffer<short> buffer_droid;
std::atomic<size_t> playing_offset;

void empty_buffer()
{
	for(size_t i = 0; i < buffer_droid.size(); ++i)
	{
		buffer_droid[i] = 0;
	}
}

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

	size_t counter = 0;
	for (uint32_t frame = 0; frame < static_cast<uint32_t>(num_samples); ++frame)
	{
		short converted = buffer_droid[playing_offset + frame];
		buffer[counter] = converted;
		buffer[counter + 1] = converted;
		counter += 2;
	}

	playing_offset += num_samples;

	return num_samples;
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_CreateBufferQueueAudioPlayer(JNIEnv* env, jobject obj, int sample_rate_arg, int buf_size_arg)
{
	noob::globals& g = noob::globals::get_instance();
	g.sample_rate = std::fabs(sample_rate_arg);
	const int buf_size = std::fabs(buf_size_arg);
	noob::logger::log(noob::importance::INFO, noob::concat("[C++] Created buffer queue player with samplerate ", noob::to_string(g.sample_rate), " and buffer size ", noob::to_string(buf_size)));

	// Setup beginning of sounds:
	playing_offset = 0;
	const size_t sound_update_hertz = 100;
	const size_t estimated_nanos_per_update = noob::nanos_per_oscillation(sound_update_hertz);	
	const size_t chunks_per_sec = noob::idiv_ceil(static_cast<size_t>(g.sample_rate), static_cast<size_t>(buf_size));
	const size_t samples_per_update_cycle = noob::idiv_ceil(static_cast<size_t>(g.sample_rate), static_cast<size_t>(sound_update_hertz));
	const size_t chunks_per_update_cycle = noob::idiv_ceil(samples_per_update_cycle, static_cast<size_t>(buf_size));
	const size_t ringbuf_size = chunks_per_update_cycle * g.sample_rate * 2;

	buffer_droid.resize(ringbuf_size);

	empty_buffer();
	buffer_droid.swap();
	empty_buffer();
	buffer_droid.swap();
	opensl_wrapper_init(audio_cb, buf_size, g.sample_rate);

	playing_audio = true;

	std::thread t([]()
			{
			auto last_time = noob::clock::now();
			size_t last_written = 0;
			
			while(true)
			{
			auto current_time = noob::clock::now();
			auto elapsed = current_time - last_time;

			// size_t samples_to_render = 

			if (playing_audio)
			{

			}

			}
			});
	t.detach();


}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_DestroyBufferQueueAudioPlayer(JNIEnv* env, jobject obj)
{
	opensl_wrapper_shutdown();
}
