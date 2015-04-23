#include "CommonIncludes.hpp"
#include "Logger.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include <GLFW/glfw3.h>
#undef None
#include <bgfx.h>
#include <bgfxplatform.h>

#include "Application.hpp"

//#include <spnav.h>

int g_gl_width = 800;
int g_gl_height = 600;
GLFWwindow* g_window = nullptr;
static application* app = nullptr;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
//	app->cam->mouse_button_callback(window, button, action, mods);
}

void cursor_callback(GLFWwindow* window, double x, double y)
{
//	app->cam->cursor_callback(window, x, y);
}

void glfw_error_callback(int error, const char* description)
{
	std::stringstream ss;
	ss << "GLFW error! error # = " << error;
	logger::log(ss.str());
	logger::log(std::string(description));	
}

void glfw_window_size_callback(GLFWwindow* window, int width, int height)
{
	g_gl_width = width;
	g_gl_height = height;
	

	bgfx::reset(width, height, BGFX_RESET_VSYNC);

	std::stringstream ss;
	ss << "Desktop: Width " << width << ", height: " << height;
	logger::log(ss.str());
	app->window_resize(width, height);
}

bool start()
{
	logger::log(std::string(("Desktop: Starting GLFW")));

	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit ())
	{
		logger::log("Desktop: ERROR - Could not start GLFW3");
		return false;
	}

//	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	g_window = glfwCreateWindow(g_gl_width, g_gl_height, "Noobwerkz editor", NULL, NULL);

	if (!g_window)
	{
		logger::log("Desktop: ERROR - Could not open window with GLFW3");
		glfwTerminate();
		return false;
	}


	glfwSetWindowSizeCallback(g_window, glfw_window_size_callback);
	glfwSetCursorPosCallback(g_window, cursor_callback);
	glfwSetMouseButtonCallback(g_window, mouse_button_callback);

	glfwMakeContextCurrent(g_window);

	
	// bgfx::renderFrame();
	glfwWindowHint(GLFW_SAMPLES, 4);

	bgfx::glfwSetWindow(g_window);
	
	app = new application();
	app->on_context_created();
	app->resume();
	app->set_archive_dir("./assets");
	logger::log("Desktop: Going into application");
	return true;
}

void _update_fps_counter(GLFWwindow* window)
{
	static double previous_seconds = glfwGetTime();
	static int frame_count;
	double current_seconds = glfwGetTime();
	double elapsed_seconds = current_seconds - previous_seconds;
	if (elapsed_seconds > 0.25)
	{
		previous_seconds = current_seconds;
		double fps = (double)frame_count / elapsed_seconds;
		char tmp[128];
		sprintf(tmp, "opengl @ fps: %.2f", fps);
		glfwSetWindowTitle (window, tmp);
		frame_count = 0;
	}
	frame_count++;
}

int main()
{
	
	logger::log("");
	logger::log("Desktop: Begin");

	start();

	while (!glfwWindowShouldClose(g_window))
	{
		// logger::log("Desktop main loop");
		// _update_fps_counter(g_window);
		bgfx::renderFrame();	
		app->step();

		glfwPollEvents();

		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_ESCAPE))
		{
			glfwSetWindowShouldClose(g_window, 1);
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_W))
		{
			app->cam->move_forward();	
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_A))
		{
			app->cam->move_west();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_D))
		{
			app->cam->move_east();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_S))
		{
			app->cam->move_back();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_Q))
		{
			app->cam->move_north();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_E))
		{
			app->cam->move_south();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_Z))
		{
			app->cam->roll_clockwise();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_C))
		{
			app->cam->roll_counterclockwise();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_LEFT))
		{
			app->cam->yaw_left();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_RIGHT))
		{
			app->cam->yaw_right();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_UP))
		{
			app->cam->pitch_up();
		}
		if (GLFW_PRESS == glfwGetKey(g_window, GLFW_KEY_DOWN))
		{
			app->cam->pitch_down();
		}
		
	//	glfwSwapBuffers(g_window);
	}

	logger::log("Desktop: Terminate");
	glfwTerminate();
	return 0;
}
