#include <memory>

#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>


#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>
#include <X11/extensions/XInput2.h>

#include "AppPrivateImplementation.hpp"
#include "Logger.hpp"
#include "SoundInterface.hpp"

namespace noob
{

	class engine_nix
	{
		public:
			bool init();
			void loop();	

		protected:

			std::string get_egl_error();
			
			// Our data members
			noob::app_facade app;
			
			// EGL-related
			EGLNativeDisplayType eglNativeDisplay;
			EGLNativeWindowType  eglNativeWindow;
			EGLDisplay  eglDisplay;
			EGLContext  eglContext;
			EGLSurface  eglSurface;
	
			// X11-related
			Window win;
			int xi_opcode;
	};

}
