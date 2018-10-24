#include <GLES3/gl.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

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
			
		protected:
			uint32_t width, height;
			EGLNativeDisplayType native_display;
			EGLNativeWindowType native_window;
			EGLDisplay display;
			EGLContext context;
			EGLSurface surface;
	};

}
