#include <atomic>

#include <bgfx.h>
#include "Graphics.hpp"
#include "Application.hpp"
#include "Logger.hpp"

#include <GLFW/glfw3.h>
#include <bgfxplatform.h>

std::atomic<uint32_t> width(1280);
std::atomic<uint32_t> height(720);

static uint32_t debug = BGFX_DEBUG_TEXT;
static uint32_t reset = BGFX_RESET_VSYNC;

static noob::application* app;

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
	noob::graphics::init(width, height);
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
		logger::log("Could not init user app");
	}


	bgfx::glfwSetWindow(window);

	bgfx::init();
	noob::graphics::init(width, height);
	app->init();

	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwMakeContextCurrent(window);

	while (!glfwWindowShouldClose(window))
	{
		app->step();
		noob::graphics::frame(width, height);
		glfwPollEvents();
	}

	// Shutdown bgfx.
	bgfx::shutdown();
	delete app;
	return 0;
}
