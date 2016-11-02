#include <string>

//#include <android/looper.h>
#include <android/window.h>

#include <EGL/egl.h>
#include <GLES3/gl3.h>

#include <jni.h>
#include <errno.h>

#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"

uint32_t _width;
uint32_t _height;

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

	if (app)
	{
		// delete app;
		app = nullptr;
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint width, jint height)
{
	noob::logger::log(noob::importance::INFO, "[C++] JNILib.OnResize()");

	_height = height;
	_width = width;

	EGLint last_context = current_context;

	current_context = reinterpret_cast<EGLint>(eglGetCurrentContext());

	// noob::graphics& gfx = noob::graphics::get_instance();

	//if (!started)
	//{
	//	gfx.init(_width, _height);
	
		app->init(_width, _height, archive_dir);

	//	started = true;
	// }

	if (current_context != last_context)
	{
		if (last_context == 0)
		 {
		app->init(_width, _height, archive_dir);
			//gfx.init(_width, _height);
		 }

	//	gfx.frame(_width, _height);
		
	//	if (app)
	//	{
	//		app->init(_width, _height, archive_dir);
	//	}
	}


	app->window_resize(_width, _height);
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	if (app)
	{
		noob::logger::log(noob::importance::INFO, "[C++] Drawing frame");
		app->step();
		//noob::graphics& gfx = noob::graphics::get_instance();
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
