#include <memory>

#include <GLES3/gl3.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>


#include "Application.hpp"


#define ES_WINDOW_RGB 0
#define ES_WINDOW_ALPHA 1
#define ES_WINDOW_DEPTH 2
#define ES_WINDOW_STENCIL 4
#define ES_WINDOW_MULTISAMPLE 8

namespace noob
{

	class engine_nix
	{
		public:
			bool init();
			void step();
		protected:

			EGLint get_context_renderable_type(EGLDisplay egl_display);
			std::string get_egl_error();

			std::unique_ptr<noob::application> app;
			uint32_t width, height;

			EGLNativeDisplayType native_display;
			EGLNativeWindowType native_window;
			EGLDisplay egl_display;
			EGLContext egl_context;
			EGLSurface egl_surface;
			Display* x_display = nullptr;
	};

}
