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

#ifndef RENDERER_H
#define RENDERER_H

#include <pthread.h>
#include <EGL/egl.h> // requires ndk r5 or newer
#include <GLES3/gl3.h>

#include "Application.hpp"
#include "android_fopen.h"

class engine_droid
{
	public:
		engine_droid();
		virtual ~engine_droid();

		// Following methods can be called from any thread.
		// They send message to render thread which executes required actions.
		void start();
		void stop();
		void setWindow(ANativeWindow* window);


	private:

		enum render_thread_message
		{
			MSG_NONE = 0,
			MSG_WINDOW_SET,
			MSG_RENDER_LOOP_EXIT
		};

		pthread_t _threadId;
		pthread_mutex_t _mutex;
		enum render_thread_message _msg;

		// android window, supported by NDK r5 and newer
		ANativeWindow* _window;
		uint32_t _width, _height;

		EGLDisplay _display;
		EGLSurface _surface;
		EGLContext _context;
		GLfloat _angle;

		noob::application* _app;

		// RenderLoop is called in a rendering thread started in start() method
		// It creates rendering context and renders scene until stop() is called
		void renderLoop();

		bool initialize();
		void destroy();

		void drawFrame();

		// Helper method for starting the thread 
		static void* threadStartCallback(void *myself);

};

#endif // RENDERER_H
