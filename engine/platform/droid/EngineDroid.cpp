#include <string>

#include <android/input.h>
#include <android/log.h>
#include <android/looper.h>
#include <android/window.h>
#include <android/log.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>

#include <jni.h>
#include <errno.h>

// #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
// #define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))


#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"

uint32_t _width;
uint32_t _height;
EGLint current_context; 
std::string archive_dir;
std::unique_ptr<noob::application> app = nullptr;

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
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_NativeSetSurface(JNIEnv* eng, jobject obj, jobject surface);
};


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnInit()");

	if (!app)
	{
		app = std::unique_ptr<noob::application>(new noob::application());
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnShutdown(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnShutdown()");

	if(app)
	{
		// delete app;
		app = nullptr;
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint width, jint height)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnResize()");

	_height = height;
	_width = width;

	auto last_context = current_context;
	current_context = reinterpret_cast<EGLint>(eglGetCurrentContext());

	if (current_context != last_context)
	{
		if (last_context != 0)
		{
			noob::logger::log(noob::importance::INFO, "bgfx::shutdown()");
			bgfx::shutdown();
		}

		bgfx::PlatformData pd = {};
		pd.context = (void*)(uintptr_t)current_context; // eglGetCurrentContext(); // Pass the EGLContext created by GLSurfaceView.
		//pd.context = eglGetCurrentContext();
		bgfx::setPlatformData(pd);

		noob::logger::log(noob::importance::INFO, "BGFX platform data set!");

		bgfx::init();
		noob::graphics& gfx = noob::graphics::get_instance();
		gfx.init(_width, _height);

		gfx.frame(_width, _height);

		// if (app)
		// {
		//	app->init();
		// }
	}

	if (app)
	{
		app->window_resize(width, height);
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	if (app)
	{
		app->step();
		noob::graphics& gfx = noob::graphics::get_instance();
		gfx.frame(_width, _height);
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnTouch(JNIEnv* env, jobject obj, int pointerID, float x, float y, int action)
{
	noob::logger::log(noob::importance::INFO, noob::concat("JNILib.OnTouch(", noob::to_string(x), ", ", noob::to_string(y), ")"));

	if (app)
	{
		app->touch(pointerID, x, y, action);
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnPause(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnPause()");
	// Pretty much a dead callback as interfering with the app pointer crashes
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResume(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnResume()");
	
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
	const char* strChars = env->GetStringUTFChars(str,(jboolean*)0);

	std::string Result(strChars, len);
	env->ReleaseStringUTFChars(str, strChars);

	return Result;
}
/*
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
   */
JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);

	noob::logger::log(noob::importance::INFO, temp);
}
