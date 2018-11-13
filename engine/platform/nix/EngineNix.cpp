#include "Application.hpp"
#include "Graphics.hpp"
#include "EngineNix.hpp"


bool noob::engine_nix::init()
{
	// First, we setup X11
	width = 320;
	height = 200;
	
	// First, we wrangle X11 (moderately easy)
	x_display = XOpenDisplay(nullptr);

	if (x_display == nullptr)
	{
		// Log oops
		return false;
	}
	
	auto screen = DefaultScreen(x_display);
	auto root = DefaultRootWindow(x_display);

	XSetWindowAttributes swa;
	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
	auto win = XCreateWindow(x_display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

	Atom wm_delete_message = XInternAtom(x_display, "WM_DELETE_WINDOW", false);
	XSetWMProtocols(x_display, win, &wm_delete_message, 1);

	XSetWindowAttributes  xattr;
	xattr.override_redirect = false;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	XWMHints hints;
	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);

	// Make the window visible on the screen
	XMapWindow(x_display, win);
	std::string title = "Noobwerkz!";
	XStoreName(x_display, win, title.c_str());

	auto wm_state = XInternAtom(x_display, "_NET_WM_STATE", false);

	// TODO: Change
	XEvent xev;
	memset(&xev, 0, sizeof(xev));
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = false;
	XSendEvent(x_display, DefaultRootWindow(x_display), false, SubstructureNotifyMask, &xev);


	// Then we wrangle EGL

	// EGLint egl_context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

	native_window = (EGLNativeWindowType) win;
	native_display = (EGLNativeDisplayType) x_display;

	egl_display = eglGetDisplay(native_display);

	// noob::logger::log(noob::importance::INFO, get_egl_error());
	
	if (egl_display == EGL_NO_DISPLAY)
	{
		noob::logger::log(noob::importance::ERROR, "[EngineNix] Couldn't get EGL display");
		return false;
	}

	EGLint major_version, minor_version;
	if (!eglInitialize(egl_display, &major_version, &minor_version))
	{	
		noob::logger::log(noob::importance::ERROR, "[EngineNix] Couldn't init EGL display");
		return false;
	}
		
	// noob::logger::log(noob::importance::INFO, get_egl_error());
	
	noob::logger::log(noob::importance::INFO, noob::concat("[EngineNix] EGL initialized; version = ", noob::to_string(major_version), ".", noob::to_string(minor_version)));

	EGLint num_configs = 0;
	
	EGLint attrib_list[] =
       	{
	//	EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 8,
		EGL_STENCIL_SIZE, 8,
		EGL_SAMPLE_BUFFERS, EGL_DONT_CARE,
		EGL_RENDERABLE_TYPE, get_context_renderable_type(egl_display),
		EGL_NONE
	};
	
	EGLConfig egl_config;
	
	if (!eglChooseConfig(egl_display, attrib_list, &egl_config, 1, &num_configs))
	{
		noob::logger::log(noob::importance::ERROR, "Could not choose EGL config!");
		return false;
	}

	noob::logger::log(noob::importance::INFO, get_egl_error());
	
	if (num_configs < 1)
	{
		noob::logger::log(noob::importance::ERROR, "EGL config chooser returned 0 configs");
		return false;
	}
	
	egl_surface = eglCreateWindowSurface(egl_display, egl_config, native_window, NULL);
	
	noob::logger::log(noob::importance::INFO, get_egl_error());

	if (egl_surface == EGL_NO_SURFACE)
	{
		noob::logger::log(noob::importance::ERROR, "Could not create EGL surface!");
		return false;
	}

	noob::logger::log(noob::importance::INFO, get_egl_error());
	
	if (!eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("Could not make EGL context current - ", get_egl_error(), ", number of configs = ", noob::to_string(num_configs)));
		return false;
	}

	// Now we deal with our own app init code...
	
	noob::sound_interface audio_interface;
	audio_interface.init();
	
	const double dpi_x = static_cast<double>(DisplayWidth(x_display, screen)) / (static_cast<double>(DisplayWidthMM(x_display, screen)) / 25.4);
	const double dpi_y = static_cast<double>(DisplayHeight(x_display, screen)) / (static_cast<double>(DisplayHeightMM(x_display, screen))/ 25.4);

	app = std::make_unique<noob::application>();

	app->init(noob::vec2ui(width, height), noob::vec2d(dpi_x, dpi_y), "./assets/");

	noob::ndof ndof;
	ndof.run();


	return true;

}


EGLint noob::engine_nix::get_context_renderable_type(EGLDisplay egl_display)
{
	const char *extensions = eglQueryString(egl_display, EGL_EXTENSIONS);
	if (extensions != NULL && std::string(extensions).compare("EGL_KHR_create_context"))
	{
		return EGL_OPENGL_ES3_BIT_KHR;
	}
	return EGL_OPENGL_ES2_BIT;
}


void noob::engine_nix::step()
{

	// app->step();
	eglSwapBuffers(egl_display, egl_surface);
}

std::string noob::engine_nix::get_egl_error()
{
	auto val = eglGetError();

	switch (val)
	{
		case EGL_SUCCESS:
		{
			return std::string("EGL_SUCCESS");
		}
		case EGL_NOT_INITIALIZED:
		{
			return std::string("EGL_NOT_INITIALIZED");
		}
		case EGL_BAD_ACCESS:
		{
			return std::string("EGL_BAD_ACCSS");
		}
		case EGL_BAD_ALLOC:
		{
			return std::string("EGL_BAD_ALLOC");
		}
		case EGL_BAD_ATTRIBUTE:
		{
			return std::string("EGL_BAD_ATTRIBUTE");
		}
		case EGL_BAD_CONTEXT:
		{
			return std::string("EGL_BAD_CONTEXT");
		}
		case EGL_BAD_CONFIG:
		{
			return std::string("EGL_BAD_CONFIG");		
		}
		case EGL_BAD_CURRENT_SURFACE:
		{
			return std::string("EGL_BAD_CURRENT_SURFACE");
		}
		case EGL_BAD_MATCH:
		{
			return std::string("EGL_BAD_MATCH");
		}
		case EGL_BAD_PARAMETER:
		{
			return std::string("EGL_BAD_PARAMETER");
		}
		case EGL_BAD_NATIVE_PIXMAP:
		{
			return std::string("EGL_BAD_NATIVE_PIXMAP");
		}
		case EGL_BAD_NATIVE_WINDOW:
		{
			return std::string("EGL_BAD_NATIVE_WINDOW");
		}
		case EGL_CONTEXT_LOST:
		{
			return std::string("EGL_CONTEXT_LOST");
		}
		default:
		{
			return noob::concat("UNDEFINED EGL ERROR: ", noob::to_string(val));
		}
	}
}

