LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := native-activity
LOCAL_SRC_FILES := libnative-activity.so
#LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
include $(PREBUILT_SHARED_LIBRARY)
