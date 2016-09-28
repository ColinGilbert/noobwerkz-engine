LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := EngineEntry
LOCAL_SRC_FILES := libEngineEntry.so
include $(PREBUILT_SHARED_LIBRARY)
