#include <string>

#include <android/input.h>
#include <android/log.h>
#include <android/looper.h>
#include <android/window.h>
#include <android_native_app_glue.h>
#include <android/native_window.h>

#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>

#include "Application.hpp"
#include "Graphics.hpp"
#include "NoobUtils.hpp"


#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))


static ANativeWindow* droid_window;
static android_app* droid_app;


inline void android_set_window(ANativeWindow* _window)
{
	bgfx::PlatformData pd;
	pd.ndt          = NULL;
	pd.nwh          = _window;
	pd.context      = NULL;
	pd.backBuffer   = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);
}


// Our saved state data.
struct saved_state
{
	float angle;
	int32_t x;
	int32_t y;
};


// Shared state for our app.
struct engine
{
	struct android_app* app;

	ASensorManager* sensor_manager;
	const ASensor* accelerometer_sensor;
	ASensorEventQueue* sensor_event_queue;

	bool animating;
	int32_t width;
	int32_t height;
	struct saved_state state;
};


// Initialize an EGL context for the current display.
static void init_display(struct engine* engine)
{
	// initialize OpenGL ES and EGL
}


// Just the current frame in the display.
static void draw_frame(struct engine* engine)
{
	// if (engine->display == NULL)
	// {
	// No display.
	//	noob::logger::log(noob::importance::ERROR, "[NativeActivity] - Attempted to draw frame with inexistent display..."
	//      return;
	// }

	// Just fill the screen with a color.
	// glClearColor(((float)engine->state.x)/engine->width, engine->state.angle, ((float)engine->state.y)/engine->height, 1);
	// glClear(GL_COLOR_BUFFER_BIT);

	// eglSwapBuffers(engine->display, engine->surface);

}


// Tear down the EGL context currently associated with the display.
static void engine_term_display(struct engine* engine)
{
	// if (engine->display != EGL_NO_DISPLAY)
	// {
	//	eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
	//	if (engine->context != EGL_NO_CONTEXT)
	//	{
	//		eglDestroyContext(engine->display, engine->context);
	//	}
	//	if (engine->surface != EGL_NO_SURFACE)
	//	{
	//		eglDestroySurface(engine->display, engine->surface);
	//	}
	//	eglTerminate(engine->display);
	// }
	engine->animating = false;
	// engine->display = EGL_NO_DISPLAY;
	// engine->context = EGL_NO_CONTEXT;
	// engine->surface = EGL_NO_SURFACE;
}


// Process the next input event.
static int32_t handle_input(struct android_app* app, AInputEvent* event)
{
	// struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		// engine->animating = 1;
		//engine->state.x = AMotionEvent_getX(event, 0);
		//engine->state.y = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}


// Process the next main command.
static void handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
		{
			// The system has asked us to save our current state.  Do so.
			// engine->app->savedState = malloc(sizeof(struct saved_state));
			// *((struct saved_state*)engine->app->savedState) = engine->state;
			// engine->app->savedStateSize = sizeof(struct saved_state);
			break;
		}
		case APP_CMD_INIT_WINDOW:
		{
			// The window is being shown, get it ready.
			if (engine->app->window != NULL)
			{
				init_display(engine);
				draw_frame(engine);
			}
			break;
		}
		case APP_CMD_TERM_WINDOW:
		{
			// The window is being hidden or closed, clean it up.
			engine_term_display(engine);
			break;
		}
		case APP_CMD_GAINED_FOCUS:
		{	// When our app gains focus, we start monitoring the accelerometer.
			if (engine->accelerometer_sensor != NULL)
			{
				ASensorEventQueue_enableSensor(engine->sensor_event_queue, engine->accelerometer_sensor);
				// We'd like to get 60 events per second (in us).
				ASensorEventQueue_setEventRate(engine->sensor_event_queue, engine->accelerometer_sensor, (1000L/60)*1000);
			}
			break;
		}
		case APP_CMD_LOST_FOCUS:
		{
			// When our app loses focus, we stop monitoring the accelerometer. This is to avoid consuming battery while not being used.
			if (engine->accelerometer_sensor != NULL)
			{
				ASensorEventQueue_disableSensor(engine->sensor_event_queue, engine->accelerometer_sensor);
			}
			// Also stop animating.
			engine->animating = false;
			draw_frame(engine);
			break;
		}
	}
}


// This is the main entry point of a native application that is using android_native_app_glue.  It runs in its own thread with its own event loop for receiving input events and doing other things.
void android_main(struct android_app* state)
{
	struct engine engine;

	// Make sure glue isn't stripped.
	app_dummy();

	//    memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = handle_cmd;
	state->onInputEvent = handle_input;
	engine.app = state;

	// Prepare to monitor accelerometer
	engine.sensor_manager = ASensorManager_getInstance();
	engine.accelerometer_sensor = ASensorManager_getDefaultSensor(engine.sensor_manager, ASENSOR_TYPE_ACCELEROMETER);
	engine.sensor_event_queue = ASensorManager_createEventQueue(engine.sensor_manager, state->looper, LOOPER_ID_USER, NULL, NULL);

	// if (state->savedState != NULL)
	// {
		// We are starting with a previous saved state; restore from it.
		// engine.state = *(struct saved_state*)state->savedState;
	// }

	// loop waiting for stuff to do.
	while (true)
	{
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events. If animating, we loop until all events are read, then continue to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) // Colin's note: Ewww...
		{

			// Process this event.
			if (source != NULL)
			{
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER)
			{
				if (engine.accelerometer_sensor != NULL)
				{
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensor_event_queue, &event, 1) > 0)
					{
						noob::logger::log(noob::importance::INFO, noob::concat("[NativeActivity] Accelerometer: x=", noob::to_string(event.acceleration.x), " y=", noob::to_string(event.acceleration.y), " z=", noob::to_string(event.acceleration.z)));
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating)
		{
			// Drawing is throttled to the screen update rate, so there is no need to do timing here.
			draw_frame(&engine);
		}
	}
}

/*
   void handle_cmd(android_app* state, int32_t _cmd)
   {
   switch (_cmd)
   {
   case APP_CMD_INPUT_CHANGED:
// Command from main thread: the AInputQueue has changed.  Upon processing
// this command, android_app->inputQueue will be updated to the new queue
// (or NULL).
break;

case APP_CMD_INIT_WINDOW:
// Command from main thread: a new ANativeWindow is ready for use.  Upon
// receiving this command, android_app->window will contain the new window
// surface.
if (droid_window != droid_app->window)
{
droid_window = droid_app->window;
android_set_window(droid_window);

int32_t width  = ANativeWindow_getWidth(droid_window);
int32_t height = ANativeWindow_getHeight(droid_window);

noob::logger::log(noob::importance::INFO, noob::concat("ANativeWindow width = ", noob::to_string(width), ", height =", noob::to_string(height)));
// WindowHandle defaultWindow = { 0 };
//m_eventQueue.postSizeEvent(defaultWindow, width, height);

//	if (!m_thread.isRunning() )
//	{
//		m_thread.init(MainThreadEntry::threadFunc, &m_mte);
//	}
}
break;

case APP_CMD_TERM_WINDOW:
// Command from main thread: the existing ANativeWindow needs to be
// terminated.  Upon receiving this command, android_app->window still
// contains the existing window; after calling android_app_exec_cmd
// it will be set to NULL.
break;

case APP_CMD_WINDOW_RESIZED:
// Command from main thread: the current ANativeWindow has been resized.
// Please redraw with its new size.
break;

case APP_CMD_WINDOW_REDRAW_NEEDED:
// Command from main thread: the system needs that the current ANativeWindow
// be redrawn.  You should redraw the window before handing this to
// android_app_exec_cmd() in order to avoid transient drawing glitches.
break;

case APP_CMD_CONTENT_RECT_CHANGED:
// Command from main thread: the content area of the window has changed,
// such as from the soft input window being shown or hidden.  You can
// find the new content rect in android_app::contentRect.
break;

case APP_CMD_GAINED_FOCUS:
{
// Command from main thread: the app's activity window has gained
// input focus.
// WindowHandle defaultWindow = { 0 };
// m_eventQueue.postSuspendEvent(defaultWindow, Suspend::WillResume);
break;
}

case APP_CMD_LOST_FOCUS:
{
// Command from main thread: the app's activity window has lost
// input focus.
// WindowHandle defaultWindow = { 0 };
// m_eventQueue.postSuspendEvent(defaultWindow, Suspend::WillSuspend);
break;
}

case APP_CMD_CONFIG_CHANGED:
// Command from main thread: the current device configuration has changed.
break;

case APP_CMD_LOW_MEMORY:
// Command from main thread: the system is running low on memory.
// Try to reduce your memory use.
break;

case APP_CMD_START:
// Command from main thread: the app's activity has been started.
break;

case APP_CMD_RESUME:
{
	// Command from main thread: the app's activity has been resumed.
	// WindowHandle defaultWindow = { 0 };
	// m_eventQueue.postSuspendEvent(defaultWindow, Suspend::DidResume);
	break;
}

case APP_CMD_SAVE_STATE:
// Command from main thread: the app should generate a new saved state
// for itself, to restore from later if needed.  If you have saved state,
// allocate it with malloc and place it in android_app.savedState with
// the size in android_app.savedStateSize.  The will be freed for you
// later.
break;

case APP_CMD_PAUSE:
{
	// Command from main thread: the app's activity has been paused.
	// WindowHandle defaultWindow = { 0 };
	// m_eventQueue.postSuspendEvent(defaultWindow, Suspend::DidSuspend);
	break;
}

case APP_CMD_STOP:
// Command from main thread: the app's activity has been stopped.
break;

case APP_CMD_DESTROY:
// Command from main thread: the app's activity is being destroyed,
// and waiting for the app thread to clean up and exit before proceeding.
// m_eventQueue.postExitEvent();
break;
}
}


void android_main(android_app* state)
{
	app_dummy();

	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = handle_cmd;
	state->onInputEvent = handle_input;
	engine.app = state;

	// Read all pending events.
	while (1) {
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident=ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				terminate_display(&engine);
				return;
			}
		}

		// Draw the current frame
		draw_frame(&engine);
	}
}
*/
