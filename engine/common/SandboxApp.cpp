#include "Application.hpp"
#include "logo.h" // TODO: Remove this soon

void application::init()
{
	logger::log("Sandbox: Begin initialization.");
	/*
	   const GLubyte* renderer = glGetString(GL_RENDERER);
	   const GLubyte* version = glGetString(GL_VERSION);

	   {
	   std::stringstream ss;
	   ss << "Sandbox: OpenGL device is " << renderer << ", version " << version << std::flush;
	   logger::log(ss.str());
	   } */


	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = BGFX_RESET_VSYNC;
	bgfx::init();

	bgfx::reset(width, height, reset);
	// Enable debug text.
	bgfx::setDebug(debug);

	// Set view 0 clear state.

	bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);

	logger::log("Sandbox: initialized :)");
}

void application::window_resize(int w, int h)
{
	width = (float)w;
	height = (float)h;

	{
		std::stringstream ss;
		ss << "Sandbox: window_resize(" << width << ", " << height << ")";
		logger::log(ss.str());
	}

	if (height == 0) 
	{
		height = 1;
	}

	float ratio = width/height;

	proj_matrix = noob::perspective(67.0f, ratio, 0.01f, 1000.0f);

	// Fix UI, camera, etc...
}

void application::update(double delta)
{
	/* if (user_data.touch_has_started == true)
	   {

	   }
	   else user_data.touch_has_started = true; */

	// Do game logic here


	cam->update(delta);

	static float cam_x = 0;
	static float cam_y = 0;
	static float cam_z = 0;

	noob::vec3 cam_pos = cam->get_position();

	if (cam_x != cam_pos.v[0] || cam_y != cam_pos.v[1] || cam_z != cam_pos.v[2])
	{
		cam_x = cam_pos.v[0];
		cam_y = cam_pos.v[1];
		cam_z = cam_pos.v[2];
		std::stringstream ss;
		ss << "cam_pos = " << cam_x << " " << cam_y << " " << cam_z;
		logger::log(ss.str());
	}
	cam_pos = cam->get_position();
} 

void application::draw(double delta)
{
	
	// Set view 0 default viewport.
	bgfx::setViewRect(0, 0, 0, width, height);


	// This dummy draw call is here to make sure that view 0 is cleared
	// if no other draw calls are submitted to view 0.
	 bgfx::submit(0);



	bgfx::setClearColor(0,0.2,0.4,0.2,0.0);

	// Use debug font to print information about this example.
	//bgfx::dbgTextClear();
	
	/*bgfx::dbgTextImage(std::max((int)(width)/2/8, 20)-20
			, std::max((int)(height/2/16), 6)-6
			, 40
			, 12
			, s_logo
			, 160
			);


	bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/00-helloworld");
	bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Initialization and debug text.");
*/
	// Advance to next frame. Rendering thread will be kicked to 
	// process submitted rendering primitives.

	bgfx::frame();

//	logger::log("Drawing frame");
}
