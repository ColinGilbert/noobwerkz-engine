#pragma once

#include <string>
#include <memory>
#include <atomic>

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_window.h> 
#include <android/native_window_jni.h>
#include <jni.h>

#include <EGL/egl.h>
//#include <EGL/eglext.h> // TODO: Necessary? Desirable?

#include <android/log.h>

#include "Application.hpp"
#include "Font.hpp"
#include "Graphics.hpp"
#include "Logger.hpp"
#include <bgfx.h>
#include <bgfxplatform.h>

static EGLint current_context; 
std::string archive_dir;
