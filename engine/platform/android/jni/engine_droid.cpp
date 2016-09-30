#include <jni.h>
#include <errno.h>
#include <math.h>

// #include <GLES/gl.h>
#include <GLES2/gl2.h>
#include <EGL/egl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>

#include <bgfx/bgfx.h>
#include <bgfx/bgfxplatform.h>

#include "NoobUtils.hpp"
#include "Application.hpp"

#include "android_fopen.h"

struct engine
{
	bool started;

	struct android_app* app;

	noob::application* user_app;

	int32_t width;
	int32_t height;

	int32_t touchX;
	int32_t touchY;
};


// Initialize an EGL context for the current display.
// TODO tidy this up, currently it's mostly Google example code.
int init_display(struct engine* e)
{
	bgfx::PlatformData pd = {};

	pd.nwh = e->app->window;

	bgfx::setPlatformData(pd);
	bgfx::init();

	e->width = ANativeWindow_getWidth(e->app->window);
	e->height = ANativeWindow_getHeight(e->app->window);

	noob::logger::log(noob::importance::INFO, noob::concat("Initializing BGFX: Width = ", noob::to_string(e->width), ", height = ", noob::to_string(e->height)));

	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.init(e->width, e->height);


	e->user_app->init();
	e->user_app->window_resize(e->width, e->height);
	e->started = true;
}


// Just the current frame in the display.
void draw_frame(struct engine* e)
{
	e->user_app->step();
	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.frame(e->width, e->height);
}


// Tear down the EGL context currently associated with the display.
void terminate_display(struct engine* engine)
{
	bgfx::shutdown();
}

// Process the next input event.
int32_t handle_input(struct android_app* app, AInputEvent* event)
{
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
	{
		engine->touchX = AMotionEvent_getX(event, 0);
		engine->touchY = AMotionEvent_getY(event, 0);
		return 1;
	}
	return 0;
}


// Process the next main command.
void handle_cmd(struct android_app* app, int32_t cmd)
{
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd)
	{
		case APP_CMD_SAVE_STATE:
			break;
		case APP_CMD_INIT_WINDOW:
			// The window is being shown, get it ready.
			if (engine->app->window != NULL)
			{
				init_display(engine);
				draw_frame(engine);
			}
			break;
		case APP_CMD_TERM_WINDOW:
			// The window is being hidden or closed, clean it up.
			terminate_display(engine);
			break;
		case APP_CMD_LOST_FOCUS:
			draw_frame(engine);
			break;
	}
}


// Main entry point, handles events
void android_main(struct android_app* state)
{
	app_dummy();

	android_fopen_set_asset_manager(state->activity->assetManager);

	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = handle_cmd;
	state->onInputEvent = handle_input;

	engine.started = false;
	engine.app = state;
	engine.user_app = new noob::application();


	// Read all pending events.
	while (1)
	{
		int ident;
		int events;
		struct android_poll_source* source;

		while ((ident=ALooper_pollAll(0, NULL, &events,(void**)&source)) >= 0)
		{
			// Process this event.
			if (source != nullptr)
			{
				source->process(state, source);
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0)
			{
				terminate_display(&engine);
				return;
			}
		}
		if (engine.started)
		{
			// Draw the current frame
			draw_frame(&engine);
		}
	}
}
