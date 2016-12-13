#include "Graphics.hpp"
#include "Application.hpp"
#include "NoobUtils.hpp"
#include "SoundInterface.hpp"

#include "NDOF.hpp"
// #include <glad/glad.h>

#define GLFW_INCLUDE_ES3
#include <GLFW/glfw3.h>

std::atomic<uint32_t> width(1280);
std::atomic<uint32_t> height(720);

noob::application app;
// static noob::ndof ndof;

void window_close_callback(GLFWwindow* window)
{
	//TODO:
	//noob::graphics& gfx = noob::graphics::get_instance();
	//gfx.shutdown();
}

void window_size_callback(GLFWwindow* window, int w, int h)
{
	// No difference from calling framebuffer_size_callback()
}

void framebuffer_size_callback(GLFWwindow* window, int w, int h)
{
	width = std::abs(w);
	height = std::abs(h);

	noob::logger::log(noob::importance::INFO, noob::concat("[EngineDesktop] Resize to ", noob::to_string(width), ", ", noob::to_string(height)));
	app.window_resize(noob::vec2ui(width, height));
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

int main()//int /*_argc*/, char** /*_argv*/)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(width, height, "Engine Desktop", NULL, NULL);

	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	GLFWmonitor* primary_monitor = glfwGetPrimaryMonitor();
	int width_mm, height_mm;
	glfwGetMonitorPhysicalSize(primary_monitor, &width_mm, &height_mm);

	const GLFWvidmode* mode = glfwGetVideoMode(primary_monitor);

	const double dpi_x = mode->width / (width_mm / 25.4);
	const double dpi_y = mode->height / (height_mm / 25.4);
	
	glfwMakeContextCurrent(window);
	// gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	glfwSwapInterval(1);

	noob::sound_interface audio_interface;
	audio_interface.init();

	app.init(noob::vec2ui(width, height), noob::vec2d(dpi_x, dpi_y), "./assets/");
	
	noob::ndof ndof;
	ndof.run();

	while (!glfwWindowShouldClose(window))
	{
		noob::ndof::data info = ndof.get_data();
		app.accept_ndof_data(info);
		app.step();
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	return 0;
}
