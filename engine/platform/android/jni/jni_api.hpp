#pragma once

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

extern "C"
{
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeOnStart(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeOnResume(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeOnPause(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeOnStop(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
	JNIEXPORT void JNICALL Java_net_noobwerkz_engine_EngineEntry_nativeLog(JNIEnv* env, jobject obj, jstring message);
};
