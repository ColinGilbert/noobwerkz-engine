#include <EGL/egl.h>
#include <GLES3/gl3.h>
#include <jni.h>

#include <thread>
#include <mutex>
#include <condition_variable>

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

uint32_t window_width, window_height;

EGLint current_context;

std::string archive_dir;
std::unique_ptr<noob::application> app = nullptr;
static std::atomic<bool> started(false), playing_audio(false), more_audio(true);
static noob::ringbuffer<int16_t> buffer_droid;

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
	static bool displayed = false;
	if (app)
	{
		if (!displayed)
		{
			noob::logger::log(noob::importance::INFO, "[C++] Drawing frame");
			displayed = true;
		}
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


int audio_cb(int16_t* buffer, int frames_per_buffer)
{
	const int16_t* readbuf = buffer_droid.head();

	noob::index_type counter = 0;
	for (uint32_t frame = 0; frame < frames_per_buffer; ++frame)
	{
		const int16_t val = readbuf[frame];
		buffer[counter] = val;
		buffer[counter + 1] = val;
		counter += 2;
	}
	more_audio = true;

	return frames_per_buffer;
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_CreateBufferQueueAudioPlayer(JNIEnv* env, jobject obj, int sample_rate_arg, int buf_size_arg)
{
	noob::logger::log(noob::importance::INFO, noob::concat("[C++] Initializing sound!"));

	noob::globals& g = noob::globals::get_instance();
	g.sample_rate = std::fabs(sample_rate_arg);
	const int buf_size = std::fabs(buf_size_arg);

	// Setup sound:
	const double nanos_per_buffer = static_cast<double>(noob::billion) / static_cast<double>(g.sample_rate);
	const double buffers_per_sec = static_cast<double>(g.sample_rate) / static_cast<double>(buf_size);

	noob::logger::log(noob::importance::INFO, noob::concat("[C++] Created buffer queue player with samplerate ", noob::to_string(g.sample_rate), " and buffer size ", noob::to_string(buf_size), "(", noob::to_string(buffers_per_sec), " buffers per second)"));

	buffer_droid.resize(buf_size);
	buffer_droid.fill(0);
	playing_audio = true;

	opensl_wrapper_init(audio_cb, buf_size, g.sample_rate);


	std::thread t([buf_size]()
			{

			while(true)
			{
			if (more_audio)
			{
			
			const noob::time_point start = noob::clock::now();

			noob::globals& g = noob::globals::get_instance();
			g.master_mixer.tick(buf_size);

			int16_t* writebuf = buffer_droid.tail();
			for (uint32_t i = 0; i < buf_size; ++i)
			{
			const float f = g.master_mixer.output_buffer[i];
			const int16_t val = static_cast<int16_t>(f * static_cast<float>(std::numeric_limits<int16_t>::max()));
			writebuf[i] = val;
			}
			buffer_droid.swap();			
			const noob::time_point end = noob::clock::now();
			const noob::duration render_time = end - start;
			g.profile_run.sound_render_duration += render_time;
			++(g.profile_run.num_sound_callbacks);
			more_audio = false;			
			}

			}
			});

	t.detach();

}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_DestroyBufferQueueAudioPlayer(JNIEnv* env, jobject obj)
{
	opensl_wrapper_shutdown();
}
