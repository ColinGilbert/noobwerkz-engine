#pragma once

#include <string>
#include <memory>
// #include <atomic>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_window.h> 
#include <android/native_window_jni.h>
#include <android/window.h>
#include <android/log.h>
#include <jni.h>

#include <EGL/egl.h>

#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"

#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>

static uint32_t _width;
static uint32_t _height;
static EGLint current_context; 
std::string archive_dir;
