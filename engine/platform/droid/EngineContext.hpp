// Code adapted from: https://github.com/tsaarni/android-native-egl-example
// Original copyright:
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
#pragma once

#include <pthread.h>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl3.h>

#include "Application.hpp"

namespace noob
{
	class engine_context
	{
		public:
			engine_context();
			virtual ~engine_context();

			// Following methods can be called from any thread.
			// They send message to render thread which executes required actions.
			void start();
			void stop();
			void set_window(ANativeWindow* window);
			void set_archive_dir(const std::string&);

		private:

			enum render_thread_msg
			{
				MSG_NONE = 0,
				MSG_WINDOW_SET,
				MSG_RENDER_LOOP_EXIT
			};

			pthread_t our_thread_id;
			pthread_mutex_t our_mutex;
			enum render_thread_msg current_message;

			// android window, supported by NDK r5 and newer
			ANativeWindow* native_window;

			EGLDisplay egl_display;
			EGLSurface egl_surface;
			EGLContext egl_context;

			noob::application app;
			bool app_started;
			std::string prefix;

			// RenderLoop is called in a rendering thread started in start() method
			// It creates rendering context and renders scene until stop() is called
			void render_loop();

			bool initialize();
			void destroy();

			void draw_frame();

			// Helper method for starting the thread 
			static void* thread_start_callback(void *myself);
	};
}
