#include "EngineDroid.hpp"

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

static noob::application* app = nullptr;

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj)
{
	logger::log("");
	logger::log("JNILib.OnInit()");

	if (!app)
	{
		app = new noob::application();
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnShutdown(JNIEnv* env, jobject obj)
{
	logger::log("JNILib.OnShutdown()");


	if(app)
	{
		delete app;
		app = nullptr;
	}
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnResize(JNIEnv* env, jobject obj, jint width, jint height)
{
	logger::log("JNILib.OnResize()");

	_height = height;
	_width = width;

	auto last_context = current_context;
	current_context = eglGetCurrentContext();
	{
		std::stringstream ss;
		ss << "Last context: " << last_context;
		logger::log(ss.str());
	}

	{
		std::stringstream ss;
		ss << "Updated context: " << current_context;
		logger::log(ss.str());
	}

	if (current_context != last_context)
	{
		if (last_context != 0)
		{
			//	logger::log("bgfx::shutdown()");
			bgfx::shutdown();
		}

		logger::log("About to set BGFX platform data");

		bgfx::PlatformData pd;
		pd.ndt = NULL;
		pd.nwh = NULL;
		pd.context = (void*)(uintptr_t)current_context; // eglGetCurrentContext(); // Pass the EGLContext created by GLSurfaceView.
		pd.backbuffer = NULL;

		bgfx::setPlatformData(pd);

		{
			logger::log("BGFX platform data reset!");
		}

		bgfx::init();

		noob::graphics::init(width, height);

		bgfx::renderFrame(); // For Android EGL, notify BGFX to use single-thread mode // BGFX_CONFIG_MULTITHREADED=0
	}


	app->window_resize(width, height);
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	// logger::log("JNILib.OnFrame()");
	noob::graphics::draw(_width, _height);
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
	// Pretty much a dead callback as interfering with the app pointer crashes
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
	archive_dir = std::string(temp);

	{
		std::stringstream ss;
		ss << "JNILib.SetupArchiveDir(" << archive_dir << ")";
		logger::log(ss.str());
	}

	if (app)
	{
		app->set_archive_dir(archive_dir);
	}

}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);
	std::string mess = std::string(temp);

	logger::log(mess);
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_NativeSetSurface(JNIEnv* env, jobject obj, jobject surface)
{

}
