#include <memory>

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#include "Application.hpp"

namespace noob
{

	class engine_nix
	{
		public:
			bool init();
			void step();
	
		protected:
		std::string get_egl_error();

		/// Display handle
		EGLNativeDisplayType eglNativeDisplay;

		/// Window handle
		EGLNativeWindowType  eglNativeWindow;

		/// EGL display
		EGLDisplay  eglDisplay;

		/// EGL context
		EGLContext  eglContext;

		/// EGL surface
		EGLSurface  eglSurface;
		
		std::unique_ptr<noob::application> app;
	
	};

}
