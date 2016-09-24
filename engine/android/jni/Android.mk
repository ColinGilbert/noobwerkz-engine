LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := native-activity
LOCAL_SRC_FILES := libnative-activity.so
include $(PREBUILT_SHARED_LIBRARY)
