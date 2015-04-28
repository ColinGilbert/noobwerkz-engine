LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE := Engine

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
	$(LOCAL_PATH)/common \
	$(LOCAL_PATH)/bgfx/include \
	$(LOCAL_PATH)/bx/include \
	$(LOCAL_PATH)/bgfx/3rdparty \
	$(LOCAL_PATH)/bgfx/3rdparty/ib-compress \
	$(LOCAL_PATH)/bgfx/3rdparty/khronos \
	$(LOCAL_PATH)/bgfx/3rdparty/sdf \
	$(LOCAL_PATH)/bgfx/3rdparty/edtaa3 \
	$(LOCAL_PATH)/bgfx/3rdparty/forsyth-too \
	$(LOCAL_PATH)/bgfx/3rdparty/stb \
	$(LOCAL_PATH)/common/bgfx_related \
	$(LOCAL_PATH)/common/bgfx_related/font \

LOCAL_SRC_FILES := EngineDroid.cpp \
	common/Graphics.cpp \
	common/Application.cpp \
	common/SandboxApp.cpp \
	common/MathFuncs.cpp \
	common/Camera.cpp \
	common/Logger.cpp \
	common/Font.cpp \
	bgfx/3rdparty/forsyth-too/forsythtriangleorderoptimizer.cpp \
	bgfx/3rdparty/ib-compress/indexbuffercompression.cpp \
	bgfx/3rdparty/ib-compress/indexbufferdecompression.cpp \
	bgfx/3rdparty/edtaa3/edtaa3func.cpp \
	bgfx/3rdparty/stb/stb_image.c \
	common/bgfx_related/cube_atlas.cpp \
	common/bgfx_related/font/font_manager.cpp \
	common/bgfx_related/font/utf8.cpp \
	common/bgfx_related/font/text_metrics.cpp \
	common/bgfx_related/font/text_buffer_manager.cpp \
	common/bgfx_related/bounds.cpp \
	bgfx/src/amalgamated.cpp \

LOCAL_STATIC_LIBRARIES := asio \
	android_native_app_glue \
	freetype

LOCAL_LDLIBS := -landroid -llog -lEGL -lGLESv3 -ldl

include $(BUILD_SHARED_LIBRARY)

$(call import-module, freetype)
$(call import-module, android/native_app_glue)
$(call import-module, asio)
