LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := Entry
LOCAL_SRC_FILES := libEntry.so
include $(PREBUILT_SHARED_LIBRARY)
