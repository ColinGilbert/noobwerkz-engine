#pragma once

extern "C" {
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnStart(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnResume(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnPause(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeOnStop(JNIEnv* jenv, jobject obj);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeSetSurface(JNIEnv* jenv, jobject obj, jobject surface);
	JNIEXPORT void JNICALL Java_net_noobwerkz_sampleapp_NoobwerkzEngine_nativeSetArchiveDir(JNIEnv* env, jobject obj, jstring dir);
};
