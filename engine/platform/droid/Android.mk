LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := Engine

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/common \
	$(LOCAL_PATH)/bgfx/include \
	$(LOCAL_PATH)/bx/include \
	$(LOCAL_PATH)/bgfx/3rdparty/khronos \

LOCAL_SRC_FILES := EngineDroid.cpp \
	common/Graphics.cpp \
	common/Application.cpp \
	common/SandboxApp.cpp \
	common/MathFuncs.cpp \
	common/Camera.cpp \
	common/Logger.cpp \
	common/Text.cpp \
	bgfx/src/amalgamated.cpp

LOCAL_STATIC_LIBRARIES := asio \
	android_native_app_glue \
	freetype

LOCAL_LDLIBS := -landroid -llog -lEGL -lGLESv3

include $(BUILD_SHARED_LIBRARY)

$(call import-module, freetype)
$(call import-module, android/native_app_glue)
$(call import-module, asio)
