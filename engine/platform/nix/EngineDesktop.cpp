#include <atomic>

#include <bgfx/bgfx.h>

#include "Graphics.hpp"
#include "Application.hpp"
#include "NoobUtils.hpp"
// #include "NDOF.hpp"

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
#	define GLFW_EXPOSE_NATIVE_X11
#	define GLFW_EXPOSE_NATIVE_GLX
#elif BX_PLATFORM_OSX
#	define GLFW_EXPOSE_NATIVE_COCOA
#	define GLFW_EXPOSE_NATIVE_NSGL
#elif BX_PLATFORM_WINDOWS
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <bgfx/bgfxplatform.h>

std::atomic<uint32_t> width(1280);
std::atomic<uint32_t> height(720);

static uint32_t debug = BGFX_DEBUG_TEXT;
static uint32_t reset = BGFX_RESET_VSYNC;

static noob::application* app;
// static noob::ndof ndof;

void window_close_callback(GLFWwindow* window)
{
	bgfx::shutdown();
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
	//	width = w;
	//	height = h;
	// set_renderer(width, height); // No difference from calling in framebuffer_size_callback()
	// app->window_resize(width, height);
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = std::abs(w);
	height = std::abs(h);

	noob::graphics& gfx = noob::graphics::get_instance();
	gfx.init(width, height);
	//std::stringstream ss;
	//ss << "[glfw] framebuffer_size_callback " << width << ", " << height;
	//logger::log(ss.str());
	app->window_resize(width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	/*	if (action == GLFW_REPEAT || action == GLFW_PRESS)
		{
		switch (key)
		{
		case GLFW_KEY_UP:
	//	app->cam.pitch_up();
	case GLFW_KEY_DOWN:
	//	app->cam.pitch_down();
	case GLFW_KEY_LEFT:
	//	app->cam.yaw_left();
	case GLFW_KEY_RIGHT:
	//	app->cam.yaw_right();
	case GLFW_KEY_A:
	//	app->cam.move_west();
	case GLFW_KEY_D:
	//	app->cam.move_east();
	case GLFW_KEY_W:
	//	app->cam.move_north();
	case GLFW_KEY_S:
	//	app->cam.move_south();
	case GLFW_KEY_C:
	//	app->cam.roll_clockwise();
	case GLFW_KEY_Z:
	//	app->cam.roll_counterclockwise();
	}
	// logger::log("GLFW: Key pressed");
	} */

}
inline void glfwSetWindow(GLFWwindow* _window)
{
	bgfx::PlatformData pd;
#	if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	pd.ndt          = glfwGetX11Display();
	pd.nwh          = (void*)(uintptr_t)glfwGetGLXWindow(_window);
	pd.context      = glfwGetGLXContext(_window);
#	elif BX_PLATFORM_OSX
	pd.ndt          = NULL;
	pd.nwh          = glfwGetCocoaWindow(_window);
	pd.context      = glfwGetNSGLContext(_window);
#	elif BX_PLATFORM_WINDOWS
	pd.ndt          = NULL;
	pd.nwh          = glfwGetWin32Window(_window);
	pd.context      = NULL;
#	endif // BX_PLATFORM_WINDOWS
	pd.backBuffer   = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);
}

int main(int /*_argc*/, char** /*_argv*/)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Engine Desktop", NULL, NULL);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	app = new noob::application();

	if (!app)
	{
		noob::logger::log(noob::importance::ERROR, "Could not init application!");
		return 0;
	}


	glfwSetWindow(window);

	bgfx::init();

	noob::graphics& gfx = noob::graphics::get_instance();

	gfx.init(width, height);

	app->init();

	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	// Note: The ext time seems to have been there for a while and everything was fine. Now (nVidia 361.28
	//glfwMakeContextCurrent(window);

	// ndof.run();

	while (!glfwWindowShouldClose(window))
	{
		// noob::ndof::data info = ndof.get_data();
		// app->accept_ndof_data(info);
		app->step();
		noob::graphics& tmp_gfx = noob::graphics::get_instance();
		tmp_gfx.frame(width, height);
		glfwPollEvents();
	}

	// Shutdown bgfx.
	bgfx::shutdown();
	delete app;
	return 0;
}
