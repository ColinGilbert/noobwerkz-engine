#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>


#include "Application.hpp"
#include "Graphics.hpp"
#include "EngineNix.hpp"


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
	Display *display = nullptr;
	Atom wm_delete_message;

	// First, we wrangle X11
	display = XOpenDisplay(nullptr);

	if (display == nullptr)
	{
		// Log oops
		return false;
	}
	
	auto screen = DefaultScreen(display);

	root = DefaultRootWindow(display);
	swa.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
	win = XCreateWindow(display, root, 0, 0, 1280, 980, 0, CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &swa);

	wm_delete_message = XInternAtom(display, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(display, win, &wm_delete_message, 1);

	xattr.override_redirect = False;
	XChangeWindowAttributes(display, win, CWOverrideRedirect, &xattr);

	hints.input = True;
	hints.flags = InputHint;
	XSetWMHints(display, win, &hints);

	// Make the window visible on the screen
	XMapWindow(display, win);
	
	std::string title = "Noobwerkz!";
	
	XStoreName(display, win, title.c_str());

	// Get identifiers for the provided atom name strings
	wm_state = XInternAtom (display, "_NET_WM_STATE", False);

	memset(&xev, 0, sizeof(xev));

	xev.type = ClientMessage;
	xev.xclient.window = win;
	xev.xclient.message_type = wm_state;
	xev.xclient.format = 32;
	xev.xclient.data.l[0] = 1;
	xev.xclient.data.l[1] = False;
	XSendEvent(display, DefaultRootWindow(display), False, SubstructureNotifyMask, &xev);

	native_window = (EGLNativeWindowType) win;
	native_display = (EGLNativeDisplayType) display;

	const double dpi_x = static_cast<double>(DisplayWidth(display, screen)) / (static_cast<double>(DisplayWidthMM(display, screen)) / 25.4);
	const double dpi_y = static_cast<double>( DisplayHeight(display, screen)) / (static_cast<double>(DisplayHeightMM(display, screen))/ 25.4);

	// Now we deal with our own app init code...
	noob::sound_interface audio_interface;
	audio_interface.init();
	
	app = std::make_unique<noob::application>();

	app->init(noob::vec2ui(width, height), noob::vec2d(dpi_x, dpi_y), "./assets/");

	noob::ndof ndof;
	ndof.run();


	return true;

}



