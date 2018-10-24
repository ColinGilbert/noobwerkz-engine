#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#include "EngineNix.hpp"


static Display *x_display = nullptr;
static Atom wm_delete_message;

bool noob::engine_nix::init()
{

	Window root;
	XSetWindowAttributes swa;
	XSetWindowAttributes  xattr;
	Atom wm_state;
	XWMHints hints;
	XEvent xev;
	EGLConfig ecfg;
	EGLint num_config;
	Window win;

	// First, we wrangle X11
	x_display = XOpenDisplay(nullptr);

	if (x_displau == nullptr)
	{
		// Log oops
		return false;
	}

	root = defaultRootWindow(x_display);
	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
	win = XCreateWindow(x_displau, root, 0, 0, width, height, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

	wm_delete_message = XInternAtom(x_display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(x_display, win, &wm_delete_message, 1);

	xattr.override_redirect = FALSE;
	XChangeWindowAttributes(x_display, win, CWOverrideRedirect, &xattr);

	hints.input = TRUE;
	hints.flags = InputHint;
	XSetWMHints(x_display, win, &hints);

	// Make the window visible on the screen
	XMapWindow(x_display, win);
	XStoreName(x_display, win, title);

	// Get identifiers for the provided atom name strings
	auto wm_state = XInternAtom (x_display, "_NET_WM_STATE", FALSE);

	memset(&xev, 0, sizeof(xev));

	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = FALSE;
	XSendEvent(x_display, DefaultRootWindow(x_display), FALSE, SubstructureNotifyMask, &xev);

	native_window = (EGLNativeWindowType) win;
	native_display = (EGLNativeDisplayType) x_display;



	// Now we deal with our own app init code...
	noob::sound_interface audio_interface;
	audio_interface.init();

	app.init(noob::vec2ui(width, height), noob::vec2d(dpi_x, dpi_y), "./assets/");

	noob::ndof ndof;
	ndof.run();


	return true;

}
