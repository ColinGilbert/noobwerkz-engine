#include "EngineNix.hpp"

// X11 related local variables
static Display *x_display = NULL;
static Atom s_wmDeleteMessage;



bool noob::engine_nix::init()
{
	Window root;
	XSetWindowAttributes swa;
	XSetWindowAttributes xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;

	EGLConfig ecfg;
	EGLint num_config;
	EGLint width = 800;
	EGLint height = 640;

	std::string title = "noobwerkz!";

	// X11 native display initialization

	x_display = XOpenDisplay(NULL);
	if (x_display == NULL)
	{
		return EGL_FALSE;
	}

	root = DefaultRootWindow(x_display);

	swa.event_mask  =  ExposureMask /* | PointerMotionMask | KeyPressMask */ | VisibilityNotify | ConfigureNotify;
	win = XCreateWindow(x_display, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

	s_wmDeleteMessage = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x_display, win, &s_wmDeleteMessage, 1);

	xattr.override_redirect = False;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);

	// Make the window visible on the screen
	XMapWindow(x_display, win);
	XStoreName(x_display, win, title.c_str());

	// Get identifiers for the provided atom name strings
	wm_state = XInternAtom (x_display, "_NET_WM_STATE", False);

	xev = {0};
	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = False;
	XSendEvent(x_display, DefaultRootWindow(x_display), False, SubstructureNotifyMask, &xev);

	int xi_query_event, xi_query_error;
	if (!XQueryExtension(x_display, "XInputExtension", &xi_opcode, &xi_query_event, &xi_query_error))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[EngineNix] Xinput extension not available - Event ", noob::to_string(xi_query_event), ", error ", noob::to_string(xi_query_error)));
		return false;
	}

	noob::logger::log(noob::importance::INFO, "[EngineNix] Prior to init xi2");

	int major, minor;
	int rc;

	/* We support XI 2.2 */
	major = 2;
	minor = 2;

	rc = XIQueryVersion(x_display, &major, &minor);
	if (rc == BadRequest)
	{
		noob::logger::log(noob::importance::INFO, noob::concat("[EngineNix] XI2 not supported. Supported: ", noob::to_string(major), ",", noob::to_string(minor), "."));
		return false;
	} else if (rc != Success)
	{
		noob::logger::log(noob::importance::ERROR, "[EngineNix] X11 (internal) bug detected");
		return false;
	}


	XIEventMask evmasks[1] = {0};
	unsigned char mask1[(XI_LASTEVENT + 7)/8] = {0};

	//memset(mask1, 0, sizeof(mask1));

	/* select for button and key events from all master devices */
	XISetMask(mask1, XI_RawMotion);
	XISetMask(mask1, XI_RawKeyPress);
	XISetMask(mask1, XI_RawKeyRelease);

	evmasks[0].deviceid = XIAllMasterDevices;
	evmasks[0].mask_len = sizeof(mask1);
	evmasks[0].mask = mask1;

	XISelectEvents(x_display, root, evmasks, 1);
	XFlush(x_display);

	// Now, onto EGL


	eglNativeWindow = (EGLNativeWindowType) win;
	eglNativeDisplay = (EGLNativeDisplayType) x_display;

	EGLConfig config;
	EGLint majorVersion;
	EGLint minorVersion;
	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE };

	eglDisplay = eglGetDisplay(eglNativeDisplay);

	// printf(get_egl_error().c_str());
	if (eglDisplay == EGL_NO_DISPLAY)
	{
		// printf("ERROR: Couldn't get EGL display.\n");
		return false;
	}

	// printf(get_egl_error().c_str());
	// Initialize EGL
	if (!eglInitialize(eglDisplay, &majorVersion, &minorVersion))
	{
		// printf("ERROR: Couldn't init EGL.\n");
		return false;
	}

	// printf(get_egl_error().c_str());

	EGLint numConfigs = 0;
	EGLint attribList[] =
	{
		EGL_RED_SIZE,       8,
		EGL_GREEN_SIZE,     8,
		EGL_BLUE_SIZE,      8,
		EGL_ALPHA_SIZE,     8,
		EGL_DEPTH_SIZE,     8,
		EGL_STENCIL_SIZE,   8,
		EGL_SAMPLE_BUFFERS, 0,
		// if EGL_KHR_create_context extension is supported, then we will use
		// EGL_OPENGL_ES3_BIT_KHR instead of EGL_OPENGL_ES2_BIT in the attribute list
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	// printf(get_egl_error().c_str());

	// Choose config
	if (!eglChooseConfig(eglDisplay, attribList, &config, 1, &numConfigs))
	{
		// printf("ERROR: Couldn't choose EGL config.\n");
		return false;
	}

	// printf(get_egl_error().c_str());
	if (numConfigs < 1)
	{
		// printf("ERROR: No configs found!\n");
		return false;
	}

	// printf(get_egl_error().c_str());

	// Create a surface
	eglSurface = eglCreateWindowSurface(eglDisplay, config, eglNativeWindow, NULL);

	// printf(get_egl_error().c_str());
	if (eglSurface == EGL_NO_SURFACE)
	{
		// printf("ERROR: No EGL surface.\n");
		return false;
	}

	// printf(get_egl_error().c_str());

	// Create a GL context
	eglContext = eglCreateContext(eglDisplay, config, EGL_NO_CONTEXT, contextAttribs);

	// printf(get_egl_error().c_str());
	if (eglContext == EGL_NO_CONTEXT)
	{
		// printf("ERROR: No EGL Context!\n");
		return false;
	}

	// printf(get_egl_error().c_str());

	// Make the context current
	if (!eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext))
	{
		return false;
		// printf("ERROR: Couldn't make egl context current!\n");
	}


	// Now, we init our app!
	noob::sound_interface audio;
	audio.init();

	auto screen = DefaultScreen(x_display);

	const double dpi_x = static_cast<double>(DisplayWidth(x_display, screen) / static_cast<double>(DisplayWidthMM(x_display, screen)) / 25.4);
	const double dpi_y = static_cast<double>(DisplayHeight(x_display, screen) / static_cast<double>(DisplayHeightMM(x_display, screen)) / 25.4);

	app.init(width, height, dpi_x, dpi_y, "./assets/");

	ndof.run();

	return true;
}

void noob::engine_nix::loop()
{
	bool running = true;
	while (running)
	{
		if (XPending(x_display))
		{
			XEvent ev;
			XIRawEvent *re;

			XGenericEventCookie *cookie = static_cast<XGenericEventCookie*>(&ev.xcookie);
			XNextEvent(x_display, &ev);

			if (XGetEventData(x_display, cookie) && cookie->extension == xi_opcode)
			{
				// TODO: Add RawEvent handling code
				switch(cookie->type)
				{
					case (XI_RawKeyRelease):
						{

							break;
						}
					case (XI_RawKeyPress):
						{

							break;
						}
					case (XI_RawMotion):
						{
							break;
						}
				};
			}
			else
			{
				switch (ev.type)
				{
					case(Expose):
						{
							XWindowAttributes attribs;
							// Add checking code
							XGetWindowAttributes(x_display, win, &attribs);
							app.window_resize(attribs.width, attribs.height);
							break;
						}
					case(VisibilityNotify):
						{
							break;
						}
					default:
						{
							break;
						}
				};
			}
		}
	
		app.accept_ndof_data(ndof.get_data());
		
		app.step();
		eglSwapBuffers(eglDisplay, eglSurface);
	}
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
				return std::string("UNDEFINED EGL ERROR");
			}
	}
}
