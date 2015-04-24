#include "Activity.h"

class SampleActivity : public Activity
{
	public:
		SampleActivity (android_app* app) : Activity(app) {}

	protected:
		void postInit()
		{
			logger::log("postInit()");
			// Initialize GL state.
			/*
			   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
			   glEnable(GL_CULL_FACE);
			   glShadeModel(GL_SMOOTH);
			   glDisable(GL_DEPTH_TEST);

			   glClearColor(0.5,0.5,0.5,1);
			   */
			uint32_t debug = BGFX_DEBUG_TEXT;
			uint32_t reset = BGFX_RESET_VSYNC;

			bgfx::init();	
			bgfx::reset(width, height, reset);

			// Enable debug text.
			bgfx::setDebug(debug);

			// Set view 0 clear state.
			bgfx::setViewClear(0
					, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
					, 0x703070ff
					, 1.0f
					, 0
					);
		}

		void drawImpl()
		{
			logger::log("Draw");
			bgfx::setViewRect(0, 0, 0, width, height);

			// This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::submit(0);

			// Use debug font to print information about this example.
			bgfx::dbgTextClear();
			bgfx::dbgTextPrintf(0, 1, 0x4f, "bgfx/examples/00-helloworld");
			bgfx::dbgTextPrintf(0, 2, 0x6f, "Description: Initialization and debug text.");

			// Advance to next frame. Rendering thread will be kicked to
			// process submitted rendering primitives.

			bgfx::frame();

			/*
			   glClear(GL_COLOR_BUFFER_BIT); 

			   glEnableClientState(GL_VERTEX_ARRAY);
#define fX(x) ((int)(x * (1  << 16)))
static int verts[6] = {
0,0,
fX(1),0,
0,fX(1)
};
glVertexPointer(2, GL_FIXED, 0, verts);
glColor4f(1,0,0,1);
glDrawArrays(GL_TRIANGLES, 0, 3);
*/
			}
void simulate(double elapsedS)
{
}
};

void android_main (android_app* app)
{
	app_dummy();
	SampleActivity activity(app);
	activity.run();
}
