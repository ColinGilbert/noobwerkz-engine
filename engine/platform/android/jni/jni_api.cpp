//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdint.h>
#include <jni.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#include "jniapi.h"
#include "logger.h"
#include "engine_droid.hpp"


static ANativeWindow *window = 0;
static Renderer *renderer = 0;

JNIEXPORT void JNICALL net_noobwerkz_engine_EngineEntry_nativeOnStart(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnStart");
    renderer = new Renderer();
    return;
}

JNIEXPORT void JNICALL net_noobwerkz_engine_EngineEntry_nativeOnResume(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnResume");
    renderer->start();
    return;
}

JNIEXPORT void JNICALL net_noobwerkz_engine_EngineEntry_nativeOnPause(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnPause");
    renderer->stop();
    return;
}

JNIEXPORT void JNICALL net_noobwerkz_engine_EngineEntry_nativeOnStop(JNIEnv* jenv, jobject obj)
{
    LOG_INFO("nativeOnStop");
    delete renderer;
    renderer = 0;
    return;
}

JNIEXPORT void JNICALL net_noobwerkz_engine_EngineEntry_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
    if (surface != 0)
    {
        window = ANativeWindow_fromSurface(jenv, surface);
	noob::logger::log(noob::importance::INFO, noob::concat("Got window ", noob::to_string(window), "."));
        renderer->setWindow(window);
    }
    else
    {
        noob::logger::log(noob::importance::INFO, "Releasing window.");
        ANativeWindow_release(window);
    }
}


std::string ConvertJString(JNIEnv* env, jstring str)
{
	if (!str)
	{
		return std::string();
	}

	const jsize len = env->GetStringUTFLength(str);
	const char* msg = env->GetStringUTFChars(str,(jboolean*)0);

	std::string results(msg, len);
	env->ReleaseStringUTFChars(str, msg);

	return results;
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

JNIEXPORT void JNICALL  net_noobwerkz_engine_EngineEntry_nativeLog(JNIEnv* env, jobject obj, jstring message)
{
	const char* temp = env->GetStringUTFChars(message, NULL);

	noob::logger::log(noob::importance::INFO, temp);
}
