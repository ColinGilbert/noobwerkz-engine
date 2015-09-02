#include <atomic>

#include <bgfx.h>
#include "Graphics.hpp"
#include "Application.hpp"
#include "Logger.hpp"
#include "NDOF.hpp"
#include "Keys.hpp"

#include <GLFW/glfw3.h>
#include <bgfxplatform.h>

std::atomic<uint32_t> width(1280);
std::atomic<uint32_t> height(720);

static uint32_t debug = BGFX_DEBUG_TEXT;
static uint32_t reset = BGFX_RESET_VSYNC;

static noob::application* app;
static noob::ndof ndof;

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
	app->window_resize(width, height);
}

noob::key_state to_keystate(int action)
{
	switch (action)
	{
		case GLFW_RELEASE:
			return noob::key_state::RELEASE;
		case GLFW_REPEAT:
			return noob::key_state::REPEAT;
		case GLFW_PRESS:
			return noob::key_state::PRESS;
		default:
			return noob::key_state::UNKNOWN;
}
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key)
	{
		case GLFW_KEY_SPACE:
			app->key_input(noob::key_type::SPACE, to_keystate(action));
			break;
		case GLFW_KEY_APOSTROPHE:
			app->key_input(noob::key_type::APOSTROPHE, to_keystate(action));
			break;
		case GLFW_KEY_COMMA:
			app->key_input(noob::key_type::COMMA, to_keystate(action));
			break;
		case GLFW_KEY_MINUS:
			app->key_input(noob::key_type::MINUS, to_keystate(action));
			break;
		case GLFW_KEY_PERIOD:
			app->key_input(noob::key_type::PERIOD, to_keystate(action));
			break;
		case GLFW_KEY_SLASH:
			app->key_input(noob::key_type::SLASH, to_keystate(action));
			break;
		case GLFW_KEY_0:
			app->key_input(noob::key_type::_0, to_keystate(action));
			break;
		case GLFW_KEY_1:
			app->key_input(noob::key_type::_1, to_keystate(action));
			break;
		case GLFW_KEY_2:
			app->key_input(noob::key_type::_2, to_keystate(action));
			break;
		case GLFW_KEY_3:
			app->key_input(noob::key_type::_3, to_keystate(action));
			break;
		case GLFW_KEY_4:
			app->key_input(noob::key_type::_4, to_keystate(action));
			break;
		case GLFW_KEY_5:
			app->key_input(noob::key_type::_5, to_keystate(action));
			break;
		case GLFW_KEY_6:
			app->key_input(noob::key_type::_6, to_keystate(action));
			break;
		case GLFW_KEY_7:
			app->key_input(noob::key_type::_7, to_keystate(action));
			break;
		case GLFW_KEY_8:
			app->key_input(noob::key_type::_8, to_keystate(action));
			break;
		case GLFW_KEY_9:
			app->key_input(noob::key_type::_9, to_keystate(action));
			break;
		case GLFW_KEY_SEMICOLON:
			app->key_input(noob::key_type::SEMICOLON, to_keystate(action));
			break;
		case GLFW_KEY_EQUAL:
			app->key_input(noob::key_type::EQUAL, to_keystate(action));
			break;
		case GLFW_KEY_A:
			app->key_input(noob::key_type::A, to_keystate(action));
			break;
		case GLFW_KEY_B:
			app->key_input(noob::key_type::B, to_keystate(action));
			break;
		case GLFW_KEY_C:
			app->key_input(noob::key_type::C, to_keystate(action));
			break;
		case GLFW_KEY_D:
			app->key_input(noob::key_type::D, to_keystate(action));
			break;
		case GLFW_KEY_E:
			app->key_input(noob::key_type::E, to_keystate(action));
			break;
		case GLFW_KEY_F:
			app->key_input(noob::key_type::F, to_keystate(action));
			break;
		case GLFW_KEY_G:
			app->key_input(noob::key_type::G, to_keystate(action));
			break;
		case GLFW_KEY_H:
			app->key_input(noob::key_type::H, to_keystate(action));
			break;
		case GLFW_KEY_I:
			app->key_input(noob::key_type::I, to_keystate(action));
			break;
		case GLFW_KEY_J:
			app->key_input(noob::key_type::J, to_keystate(action));
			break;
		case GLFW_KEY_K:
			app->key_input(noob::key_type::K, to_keystate(action));
			break;
		case GLFW_KEY_L:
			app->key_input(noob::key_type::L, to_keystate(action));
			break;
		case GLFW_KEY_M:
			app->key_input(noob::key_type::M, to_keystate(action));
			break;
		case GLFW_KEY_N:
			app->key_input(noob::key_type::N, to_keystate(action));
			break;
		case GLFW_KEY_O:
			app->key_input(noob::key_type::O, to_keystate(action));
			break;
		case GLFW_KEY_P:
			app->key_input(noob::key_type::P, to_keystate(action));
			break;
		case GLFW_KEY_Q:
			app->key_input(noob::key_type::Q, to_keystate(action));
			break;
		case GLFW_KEY_R:
			app->key_input(noob::key_type::R, to_keystate(action));
			break;
		case GLFW_KEY_U:
			app->key_input(noob::key_type::U, to_keystate(action));
			break;
		case GLFW_KEY_V:
			app->key_input(noob::key_type::V, to_keystate(action));
			break;
		case GLFW_KEY_W:
			app->key_input(noob::key_type::W, to_keystate(action));
			break;
		case GLFW_KEY_X:
			app->key_input(noob::key_type::X, to_keystate(action));
			break;
		case GLFW_KEY_Y:
			app->key_input(noob::key_type::Y, to_keystate(action));
			break;
		case GLFW_KEY_Z:
			app->key_input(noob::key_type::Z, to_keystate(action));
			break;
		case GLFW_KEY_LEFT_BRACKET:
			app->key_input(noob::key_type::LEFT_BRACKET, to_keystate(action));
			break;
		case GLFW_KEY_BACKSLASH:
			app->key_input(noob::key_type::BACKSLASH, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT_BRACKET:
			app->key_input(noob::key_type::RIGHT_BRACKET, to_keystate(action));
			break;
		case GLFW_KEY_GRAVE_ACCENT:
			app->key_input(noob::key_type::GRAVE_ACCENT, to_keystate(action));
			break;
		case GLFW_KEY_ESCAPE:
			app->key_input(noob::key_type::ESC, to_keystate(action));
			break;
		case GLFW_KEY_ENTER:
			app->key_input(noob::key_type::ENTER, to_keystate(action));
			break;
		case GLFW_KEY_TAB:
			app->key_input(noob::key_type::TAB, to_keystate(action));
			break;
		case GLFW_KEY_BACKSPACE:
			app->key_input(noob::key_type::BACKSPACE, to_keystate(action));
			break;
		case GLFW_KEY_INSERT:
			app->key_input(noob::key_type::INSERT, to_keystate(action));
			break;
		case GLFW_KEY_DELETE:
			app->key_input(noob::key_type::DELETE, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT:
			app->key_input(noob::key_type::RIGHT_ARROW, to_keystate(action));
			break;
		case GLFW_KEY_LEFT:
			app->key_input(noob::key_type::LEFT_ARROW, to_keystate(action));
			break;
		case GLFW_KEY_DOWN:
			app->key_input(noob::key_type::DOWN_ARROW, to_keystate(action));
			break;
		case GLFW_KEY_UP:
			app->key_input(noob::key_type::UP_ARROW, to_keystate(action));
			break;
		case GLFW_KEY_PAGE_UP:
			app->key_input(noob::key_type::PAGE_UP, to_keystate(action));
			break;
		case GLFW_KEY_PAGE_DOWN:
			app->key_input(noob::key_type::PAGE_DOWN, to_keystate(action));
			break;
		case GLFW_KEY_HOME:
			app->key_input(noob::key_type::HOME, to_keystate(action));
			break;
		case GLFW_KEY_END:
			app->key_input(noob::key_type::END, to_keystate(action));
			break;
		case GLFW_KEY_CAPS_LOCK:
			app->key_input(noob::key_type::CAPLOCK, to_keystate(action));
			break;
		case GLFW_KEY_SCROLL_LOCK:
			app->key_input(noob::key_type::SCROLL_LOCK, to_keystate(action));
			break;
		case GLFW_KEY_NUM_LOCK:
			app->key_input(noob::key_type::NUM_LOCK, to_keystate(action));
			break;
		case GLFW_KEY_PRINT_SCREEN:
			app->key_input(noob::key_type::PRINT_SCREEN, to_keystate(action));
			break;
		case GLFW_KEY_PAUSE:
			app->key_input(noob::key_type::PAUSE, to_keystate(action));
			break;
		case GLFW_KEY_F1:
			app->key_input(noob::key_type::F1, to_keystate(action));
			break;
		case GLFW_KEY_F2:
			app->key_input(noob::key_type::F2, to_keystate(action));
			break;
		case GLFW_KEY_F3:
			app->key_input(noob::key_type::F3, to_keystate(action));
			break;
		case GLFW_KEY_F4:
			app->key_input(noob::key_type::F4, to_keystate(action));
			break;
		case GLFW_KEY_F5:
			app->key_input(noob::key_type::F5, to_keystate(action));
			break;
		case GLFW_KEY_F6:
			app->key_input(noob::key_type::F6, to_keystate(action));
			break;
		case GLFW_KEY_F7:
			app->key_input(noob::key_type::F7, to_keystate(action));
			break;
		case GLFW_KEY_F8:
			app->key_input(noob::key_type::F8, to_keystate(action));
			break;
		case GLFW_KEY_F9:
			app->key_input(noob::key_type::F9, to_keystate(action));
			break;
		case GLFW_KEY_F10:
			app->key_input(noob::key_type::F10, to_keystate(action));
			break;
		case GLFW_KEY_F11:
			app->key_input(noob::key_type::F11, to_keystate(action));
			break;
		case GLFW_KEY_F12:
			app->key_input(noob::key_type::F12, to_keystate(action));
			break;
		case GLFW_KEY_KP_0:
			app->key_input(noob::key_type::NUMPAD_0, to_keystate(action));
			break;
		case GLFW_KEY_KP_1:
			app->key_input(noob::key_type::NUMPAD_1, to_keystate(action));
			break;
		case GLFW_KEY_KP_2:
			app->key_input(noob::key_type::NUMPAD_2, to_keystate(action));
			break;		
		case GLFW_KEY_KP_3:
			app->key_input(noob::key_type::NUMPAD_3, to_keystate(action));
			break;
		case GLFW_KEY_KP_4:
			app->key_input(noob::key_type::NUMPAD_4, to_keystate(action));
			break;
		case GLFW_KEY_KP_5:
			app->key_input(noob::key_type::NUMPAD_5, to_keystate(action));
			break;
		case GLFW_KEY_KP_6:
			app->key_input(noob::key_type::NUMPAD_6, to_keystate(action));
			break;
		case GLFW_KEY_KP_7:
			app->key_input(noob::key_type::NUMPAD_7, to_keystate(action));
			break;
		case GLFW_KEY_KP_8:
			app->key_input(noob::key_type::NUMPAD_8, to_keystate(action));
			break;
		case GLFW_KEY_KP_9:
			app->key_input(noob::key_type::NUMPAD_9, to_keystate(action));
			break;
		case GLFW_KEY_KP_DECIMAL:
			app->key_input(noob::key_type::NUMPAD_DOT, to_keystate(action));
			break;
		case GLFW_KEY_KP_DIVIDE:
			app->key_input(noob::key_type::NUMPAD_DIV, to_keystate(action));
			break;
		case GLFW_KEY_KP_MULTIPLY:
			app->key_input(noob::key_type::NUMPAD_MULT, to_keystate(action));
			break;
		case GLFW_KEY_KP_SUBTRACT:
			app->key_input(noob::key_type::NUMPAD_SUB, to_keystate(action));
			break;
		case GLFW_KEY_KP_ADD:
			app->key_input(noob::key_type::NUMPAD_ADD, to_keystate(action));
			break;
		case GLFW_KEY_KP_ENTER:
			app->key_input(noob::key_type::NUMPAD_ENTER, to_keystate(action));
			break;
		case GLFW_KEY_KP_EQUAL:
			app->key_input(noob::key_type::NUMPAD_EQUAL, to_keystate(action));
			break;
		case GLFW_KEY_LEFT_SHIFT:
			app->key_input(noob::key_type::SHIFT_LEFT, to_keystate(action));
			break;
		case GLFW_KEY_LEFT_CONTROL:
			app->key_input(noob::key_type::CTRL_LEFT, to_keystate(action));
			break;
		case GLFW_KEY_LEFT_ALT:
			app->key_input(noob::key_type::ALT_LEFT, to_keystate(action));
			break;
		case GLFW_KEY_LEFT_SUPER:
			app->key_input(noob::key_type::SUPER_LEFT, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT_SHIFT:
			app->key_input(noob::key_type::SHIFT_RIGHT, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT_CONTROL:
			app->key_input(noob::key_type::CTRL_RIGHT, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT_ALT:
			app->key_input(noob::key_type::ALT_RIGHT, to_keystate(action));
			break;
		case GLFW_KEY_RIGHT_SUPER:
			app->key_input(noob::key_type::SUPER_RIGHT, to_keystate(action));
			break;
		case GLFW_KEY_MENU:
			app->key_input(noob::key_type::MENU, to_keystate(action));
			break;
		default:
			app->key_input(noob::key_type::UNKNOWN, to_keystate(action));
	}
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

	ndof.run();

	while (!glfwWindowShouldClose(window))
	{
		noob::ndof::data info = ndof.get_data();
		app->accept_ndof_data(info);
		app->step();
		noob::graphics::frame(width, height);
		glfwPollEvents();
	}

	// Shutdown bgfx.
	bgfx::shutdown();
	delete app;
	return 0;
}
