// Code adapted from: https://github.com/tsaarni/android-native-egl-example
// Original copyright:
//
//
// Copyright 2011 Tero Saarni
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <android/native_window.h> // requires ndk r5 or newer
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl3.h>

#include "NoobUtils.hpp"
#include "EngineContext.hpp"


noob::engine_context::engine_context() : current_message(MSG_NONE), egl_display(0), egl_surface(0), egl_context(0), app_started(false), prefix("")
{
	noob::logger::log(noob::importance::INFO, "engine_context instance created");
	pthread_mutex_init(&our_mutex, 0);    
}


noob::engine_context::~engine_context()
{
	noob::logger::log(noob::importance::INFO, "engine_context instance destroyed");
	pthread_mutex_destroy(&our_mutex);
}


void noob::engine_context::start()
{
	noob::logger::log(noob::importance::INFO, "Creating renderer thread");
	pthread_create(&our_thread_id, 0, thread_start_callback, this);
}


void noob::engine_context::stop()
{
	noob::logger::log(noob::importance::INFO, "Stopping renderer thread");

	// send message to render thread to stop rendering
	pthread_mutex_lock(&our_mutex);
	current_message = MSG_RENDER_LOOP_EXIT;
	pthread_mutex_unlock(&our_mutex);    

	pthread_join(our_thread_id, 0);
	noob::logger::log(noob::importance::INFO, "engine_context thread stopped");
}


void noob::engine_context::set_window(ANativeWindow *window)
{
	// notify render thread that window has changed
	pthread_mutex_lock(&our_mutex);
	current_message = MSG_WINDOW_SET;
	native_window = window;
	pthread_mutex_unlock(&our_mutex);
}


void noob::engine_context::set_archive_dir(const std::string& arg)
{
	prefix = arg;
}


void noob::engine_context::render_loop()
{
	bool renderingEnabled = true;

	noob::logger::log(noob::importance::INFO, "render_loop()");

	while (renderingEnabled)
	{
		pthread_mutex_lock(&our_mutex);

		// process incoming messages
		switch (current_message)
		{
			case MSG_WINDOW_SET:
				initialize();
				if (!app_started)
				{
					app.init(ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window), prefix);
					app_started = true;
				}
				else
				{
					app.window_resize(ANativeWindow_getWidth(native_window), ANativeWindow_getHeight(native_window));				
				}

				break;

			case MSG_RENDER_LOOP_EXIT:
				renderingEnabled = false;
				destroy();
				break;

			default:
				break;
		}

		current_message = MSG_NONE;

		if (egl_display)
		{
			draw_frame();
			if (!eglSwapBuffers(egl_display, egl_surface))
			{
				noob::logger::log(noob::importance::ERROR, noob::concat("eglSwapBuffers() returned error ", noob::to_string(eglGetError())));
			}
		}

		pthread_mutex_unlock(&our_mutex);
	}

	// noob::logger::log(noob::importance::INFO, "Render loop exits");
}


bool noob::engine_context::initialize()
{
	const EGLint attribs[] =
	{
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 5,
		EGL_GREEN_SIZE, 6,
		EGL_RED_SIZE, 5,
		// EGL_ALPHA_SIZE, 8,
		// EGL_DEPTH_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	const EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

	EGLDisplay display;
	EGLConfig config;    
	EGLint num_configs;
	EGLint format;
	EGLSurface surface;
	EGLContext context;
	EGLint width;
	EGLint height;
	GLfloat ratio;

	noob::logger::log(noob::importance::INFO, "Initializing context");

	if ((display = eglGetDisplay(EGL_DEFAULT_DISPLAY)) == EGL_NO_DISPLAY)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglGetDisplay() returned error ", noob::to_string(eglGetError())));
		return false;
	}

	if (!eglInitialize(display, 0, 0))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglInitialize() returned error ", noob::to_string(eglGetError())));
		return false;
	}

	if (!eglChooseConfig(display, attribs, &config, 1, &num_configs))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglChooseConfig() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	if (!eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglGetConfigAttrib() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	ANativeWindow_setBuffersGeometry(native_window, 0, 0, format);

	if (!(surface = eglCreateWindowSurface(display, config, native_window, 0)))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglCreateWindowSurface() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	if (!(context = eglCreateContext(display, config, 0, context_attribs)))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglCreateContext() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	if (!eglMakeCurrent(display, surface, surface, context))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglMakeCurrent() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	if (!eglQuerySurface(display, surface, EGL_WIDTH, &width) || !eglQuerySurface(display, surface, EGL_HEIGHT, &height))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("eglQuerySurface() returned error ", noob::to_string(eglGetError())));
		destroy();
		return false;
	}

	egl_display = display;
	egl_surface = surface;
	egl_context = context;

	return true;
}


void noob::engine_context::destroy()
{
	noob::logger::log(noob::importance::INFO, "Destroying context");

	eglMakeCurrent(egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	eglDestroyContext(egl_display, egl_context);
	eglDestroySurface(egl_display, egl_surface);
	eglTerminate(egl_display);

	egl_display = EGL_NO_DISPLAY;
	egl_surface = EGL_NO_SURFACE;
	egl_context = EGL_NO_CONTEXT;
}


void noob::engine_context::draw_frame()
{
	noob::logger::log(noob::importance::INFO, "Draw frame");
	if (app_started)
	{
		app.step();
	}
}


void* noob::engine_context::thread_start_callback(void *myself)
{
	engine_context *renderer = (engine_context*)myself;

	renderer->render_loop();
	pthread_exit(0);

	return nullptr;
}
