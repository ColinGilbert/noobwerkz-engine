LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm

LOCAL_MODULE:= freetype 

LOCAL_C_INCLUDES := \
$(LOCAL_PATH)/builds \
$(LOCAL_PATH)/include \
$(LOCAL_PATH)/devel

LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES:= \
src/base/ftbbox.c \
src/base/ftbitmap.c \
src/base/ftglyph.c \
src/base/ftstroke.c \
src/base/ftxf86.c \
src/base/ftbase.c \
src/base/ftsystem.c \
src/base/ftinit.c \
src/base/ftgasp.c \
src/raster/raster.c \
src/sfnt/sfnt.c \
src/smooth/smooth.c \
src/autofit/autofit.c \
src/truetype/truetype.c \
src/cff/cff.c \
src/pcf/pcf.c \
src/bdf/bdf.c \
src/pfr/pfr.c \
src/psnames/psnames.c \
src/pshinter/pshinter.c \
src/gzip/ftgzip.c \
src/type42/type42.c \
src/winfonts/winfnt.c \
src/type1/type1.c \
src/psaux/psaux.c \
src/lzw/ftlzw.c \
src/lzw/ftzopen.c \
src/otvalid/otvalid.c \
src/cid/type1cid.c \

LOCAL_CFLAGS += -W -Wall
LOCAL_CFLAGS += -fPIC -DPIC
LOCAL_CFLAGS += "-DDARWIN_NO_CARBON"
LOCAL_CFLAGS += "-DFT2_BUILD_LIBRARY"
 
# enable the FreeType internal memory debugger in the simulator
# you need to define the FT2_DEBUG_MEMORY environment variable
# when running the program to activate it. It will dump memory
# statistics when FT_Done_FreeType is called

ifeq ($(TARGET_SIMULATOR),true)
LOCAL_CFLAGS += "-DFT_DEBUG_MEMORY"
endif
 
# the following is for testing only, and should not be used in final builds
# of the product
#LOCAL_CFLAGS += "-DTT_CONFIG_OPTION_BYTECODE_INTERPRETER"
 
LOCAL_CFLAGS += -O2
 
include $(BUILD_STATIC_LIBRARY)
