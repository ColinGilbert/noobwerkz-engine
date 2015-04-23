#include "EngineDroid.hpp"

extern "C"
{
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj, jobject surface);
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

static application* app = nullptr;

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj, jobject surface)
{
	logger::log("");
	logger::log("JNILib.OnInit()");

	if (!app)
	{
		app = new application();
	}

	// bgfx::androidSetWindow(window);

	
	if (surface != 0)
	{
		window = ANativeWindow_fromSurface(env, surface);
		{
			std::stringstream ss;
			ss << "JNILib.init(): Got window " << window;
			logger::log(ss.str());
		}

		bgfx::androidSetWindow(window);
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnShutdown(JNIEnv* env, jobject obj)
{
	logger::log("JNILib.OnShutdown()");

	if(app)
	{
		delete app;
		app = NULL;
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint width, jint height)
{
	logger::log("JNILib.OnResize()");
	app->window_resize(width, height);
	// noob::drawing::init(width, height);
		
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	//logger::log("JNILib.OnFrame()");

	app->step();
	//noob::drawing::draw(app->get_width(), app->get_height());
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnTouch(JNIEnv* env, jobject obj, int pointerID, float x, float y, int action)
{
	{
		std::stringstream ss;
		ss << "JNILib.OnTouch(" << x << ", " << y << ")";
		logger::log(ss.str());
	}

	if(app)
	{
		app->touch(pointerID, x, y, action);
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnPause(JNIEnv* env, jobject obj)
{
	logger::log("JNILib.OnPause()");

	if(app)
	{
		app->pause();
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResume(JNIEnv* env, jobject obj)
{
	logger::log("JNILib.OnResume()");
	if(app)
	{
		app->resume();
	}
	
}

std::string ConvertJString(JNIEnv* env, jstring str)
{
	if (!str) { return std::string(); }
	const jsize len = env->GetStringUTFLength(str);
	const char* strChars = env->GetStringUTFChars(str,(jboolean*)0);
	std::string Result(strChars, len);
	env->ReleaseStringUTFChars(str, strChars);
	{
		std::stringstream ss;
		ss << "JNILib.ConvertJString(" << Result << ")";
		logger::log(ss.str());
	}
	return Result;
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_SetupArchiveDir(JNIEnv * env, jobject obj, jstring dir)
{
	const char* temp = env->GetStringUTFChars(dir, NULL);
	archiveDir = std::string(temp);

	{
		std::stringstream ss;
		ss << "JNILib.SetupArchiveDir(" << archiveDir << ")";
		logger::log(ss.str());
	}
	/*	if (app)
		{
		app->SetupArchiveDir(archiveDir);
		}
		*/
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);
	std::string mess = std::string(temp);

	logger::log(mess);
}

/*
JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_NativeSetSurface(JNIEnv* env, jobject obj, jobject surface)
{
	if (surface != 0)
	{
		window = ANativeWindow_fromSurface(env, surface);
		{
			std::stringstream ss;
			ss << "JNILib.NativeSetSurface(): Got window " << window;
			logger::log(ss.str());
		}

		bgfx::androidSetWindow(window);
	}
	else
	{
		logger::log("JNILib.NativeSetSurface(): Releasing window");
		// ANativeWindow_release(window);
	}
} */
