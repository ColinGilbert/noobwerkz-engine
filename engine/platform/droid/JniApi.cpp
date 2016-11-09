// Code adapted from: https://github.com/tsaarni/android-native-egl-example
// Original copyright:
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

#include <memory>

#include <stdint.h>
#include <jni.h>

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#include "JniApi.hpp"
#include "NoobUtils.hpp"
#include "EngineContext.hpp"

static ANativeWindow *window = 0;
static std::unique_ptr<noob::engine_context> engine;

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnStart(JNIEnv* jenv, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "nativeOnStart");
	engine = std::make_unique<noob::engine_context>();
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnResume(JNIEnv* jenv, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "nativeOnResume");
	engine->start();
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnPause(JNIEnv* jenv, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "nativeOnPause");
	engine->stop();
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnStop(JNIEnv* jenv, jobject obj)
{
	noob::logger::log(noob::importance::INFO, "nativeOnStop");
	engine.release();
}


JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface)
{
	if (surface != 0)
	{
		window = ANativeWindow_fromSurface(jenv, surface);
		noob::logger::log(noob::importance::INFO, noob::concat("Got window ", noob::to_string(reinterpret_cast<size_t>(window))));
		engine->set_window(window);
	}
	else
	{
		noob::logger::log(noob::importance::INFO, "Releasing window");
		ANativeWindow_release(window);
	}

}

JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeSetArchiveDir(JNIEnv* env, jobject obj, jstring dir)
{
	const char* temp = env->GetStringUTFChars(dir, NULL);
	const std::string archive_dir = std::string(temp);
	
	noob::logger::log(noob::importance::INFO, noob::concat("nativeSetupArchiveDir(", archive_dir, ")"));
	
	engine->set_archive_dir(archive_dir);
}
