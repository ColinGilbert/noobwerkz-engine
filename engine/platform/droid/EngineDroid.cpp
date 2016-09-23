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

static std::unique_ptr<noob::application> app; // = nullptr;

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnInit(JNIEnv* env, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "JNILib.OnInit()");

	if (!app)
	{
		app = std::unique_ptr<noob::application>(new noob::application());
	}

	// http://stackoverflow.com/questions/32028810/android-how-to-get-the-preferred-native-audio-buffer-size-and-audio-sample-r/35507038#35507038
	jclass audioSystem = env->FindClass("android/media/AudioSystem");
	jmethodID method = env->GetStaticMethodID(audioSystem, "getPrimaryOutputSamplingRate", "()I");
	jint nativeOutputSampleRate = env->CallStaticIntMethod(audioSystem, method);
	method = env->GetStaticMethodID(audioSystem, "getPrimaryOutputFrameCount", "()I");
	jint nativeBufferLength = env->CallStaticIntMethod(audioSystem, method);

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
	/*
	   auto last_context = current_context;
	   current_context = eglGetCurrentContext();
	   {
	   std::stringstream ss;
	   ss << "Last context: " << last_context;
	   noob::logger::log(noob::importance::INFO, ss.str());
	   }

	   {
	   std::stringstream ss;
	   ss << "Updated context: " << current_context;
	   noob::logger::log(noob::importance::INFO, ss.str());
	   }

	   if (current_context != last_context)
	   {
	   if (last_context != 0)
	   {
	   noob::logger::log(noob::importance::INFO, "bgfx::shutdown()");
	   bgfx::shutdown();
	   }

	   noob::logger::log(noob::importance::INFO, "About to set BGFX platform data");

	   bgfx::PlatformData pd;
	   pd.ndt = NULL; // Native display type
	   pd.nwh = NULL; // Native window handle
	   pd.context = (void*)(uintptr_t)current_context; // eglGetCurrentContext(); // Pass the EGLContext created by GLSurfaceView.
	   pd.backbuffer = NULL; // GL backbuffer, or D3D render target view

	   bgfx::setPlatformData(pd);

	   noob::logger::log(noob::importance::INFO, "BGFX platform data reset!");
	   */
	ANativeWindow* window = ANativeWindow_fromSurface(env, obj);

	bgfx::PlatformData pd;
	pd.ndt          = NULL;
	pd.nwh          = window;
	pd.context      = NULL;
	pd.backBuffer   = NULL;
	pd.backBufferDS = NULL;

	bgfx::setPlatformData(pd);

	bgfx::init();
	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.init(width, height);

	if (app)
	{
		app->init();
	}

	app->window_resize(width, height);
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnFrame(JNIEnv* env, jobject obj)
{
	app->step();
	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.frame(_width, _height);
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_OnTouch(JNIEnv* env, jobject obj, int pointerID, float x, float y, int action)
{
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
	if (!str) { return std::string(); }

	const jsize len = env->GetStringUTFLength(str);
	const char* strChars = env->GetStringUTFChars(str,(jboolean*)0);
	std::string Result(strChars, len);

	env->ReleaseStringUTFChars(str, strChars);
	// {
	// std::stringstream ss;
	// ss << "JNILib.ConvertJString(" << Result << ")";
	// noob::logger::log(noob::importance::INFO, ss.str());
	// }

	return Result;
}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_SetupArchiveDir(JNIEnv * env, jobject obj, jstring dir)
{
	const char* temp = env->GetStringUTFChars(dir, NULL);
	archive_dir = std::string(temp);

	noob::logger::log(noob::importance::INFO, noob::concat("JNILib.SetupArchiveDir(",  archive_dir, ")"));

	if (app)
	{
		app->set_archive_dir(archive_dir);
	}

}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_Log(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);
	noob::logger::log(noob::importance::INFO, std::string(temp));
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_JNILib_NativeSetSurface(JNIEnv* env, jobject obj, jobject surface)
{

}
