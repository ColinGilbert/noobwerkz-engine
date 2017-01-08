#pragma once

#include <string>
#include <tuple>

#include <GLFW/glfw3.h>

#include "Keys.hpp"

namespace noob
{
	struct key_info
	{
		std::string str;
		noob::keyboard::keys key;
	};

	// enum class keys { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, APOSTROPHE, MINUS, EQUAL, SPACE, BACKSPACE, PERIOD, COMMA, SLASH, TAB, RETURN, UP, DOWN, LEFT, RIGHT, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0, NUM_DIV, NUM_MULT, NUM_MINUS, NUM_PLUS, NUM_ENTER, PAGEUP, PAGEDOWN, ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, INSERT, DELETE, HOME, END, ALT, CTRL, SHIFT, INVALID };

	std::tuple<noob::keyboard::keys, std::string> get_key(int k)
	{
		switch(k)
		{
			case(GLFW_KEY_UNKNOWN):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_UNKNOWN");
			case(GLFW_KEY_SPACE):
				return std::make_tuple(noob::keyboard::keys::SPACE, "GLFW_KEY_SPACE");
			case(GLFW_KEY_APOSTROPHE):
				return std::make_tuple(noob::keyboard::keys::APOSTROPHE, "GLFW_KEY_APOSTROPHE");
			case(GLFW_KEY_COMMA):
				return std::make_tuple(noob::keyboard::keys::COMMA, "GLFW_KEY_COMMA");
			case(GLFW_KEY_MINUS):
				return std::make_tuple(noob::keyboard::keys::MINUS, "GLFW_KEY_MINUS");
			case(GLFW_KEY_PERIOD):
				return std::make_tuple(noob::keyboard::keys::PERIOD, "GLFW_KEY_PERIOD");
			case(GLFW_KEY_SLASH):
				return std::make_tuple(noob::keyboard::keys::SLASH, "GLFW_KEY_SLASH");
			case(GLFW_KEY_0):
				return std::make_tuple(noob::keyboard::keys::_0, "GLFW_KEY_0");
			case(GLFW_KEY_1):
				return std::make_tuple(noob::keyboard::keys::_1, "GLFW_KEY_1");
			case(GLFW_KEY_2):
				return std::make_tuple(noob::keyboard::keys::_2, "GLFW_KEY_2");
			case(GLFW_KEY_3):
				return std::make_tuple(noob::keyboard::keys::_3, "GLFW_KEY_3");
			case(GLFW_KEY_4):
				return std::make_tuple(noob::keyboard::keys::_4, "GLFW_KEY_4");
			case(GLFW_KEY_5):
				return std::make_tuple(noob::keyboard::keys::_5, "GLFW_KEY_5");
			case(GLFW_KEY_6):
				return std::make_tuple(noob::keyboard::keys::_6, "GLFW_KEY_6");
			case(GLFW_KEY_7):
				return std::make_tuple(noob::keyboard::keys::_7, "GLFW_KEY_7");
			case(GLFW_KEY_8):
				return std::make_tuple(noob::keyboard::keys::_8, "GLFW_KEY_8");
			case(GLFW_KEY_9):
				return std::make_tuple(noob::keyboard::keys::_9, "GLFW_KEY_9");
			case(GLFW_KEY_SEMICOLON):
				return std::make_tuple(noob::keyboard::keys::SEMICOLON, "GLFW_KEY_SEMICOLON");
			case(GLFW_KEY_EQUAL):
				return std::make_tuple(noob::keyboard::keys::EQUAL, "GLFW_KEY_EQUAL");
			case(GLFW_KEY_A):
				return std::make_tuple(noob::keyboard::keys::A, "GLFW_KEY_A");
			case(GLFW_KEY_B):
				return std::make_tuple(noob::keyboard::keys::B, "GLFW_KEY_B");
			case(GLFW_KEY_C):
				return std::make_tuple(noob::keyboard::keys::C, "GLFW_KEY_C");
			case(GLFW_KEY_D):
				return std::make_tuple(noob::keyboard::keys::D, "GLFW_KEY_D");
			case(GLFW_KEY_E): 
				return std::make_tuple(noob::keyboard::keys::E, "GLFW_KEY_E");
			case(GLFW_KEY_F):
				return std::make_tuple(noob::keyboard::keys::F, "GLFW_KEY_F");
			case(GLFW_KEY_G):
				return std::make_tuple(noob::keyboard::keys::G, "GLFW_KEY_G");
			case(GLFW_KEY_H): 
				return std::make_tuple(noob::keyboard::keys::H, "GLFW_KEY_H");
			case(GLFW_KEY_I):
				return std::make_tuple(noob::keyboard::keys::I, "GLFW_KEY_I");
			case(GLFW_KEY_J):
				return std::make_tuple(noob::keyboard::keys::J, "GLFW_KEY_J");
			case(GLFW_KEY_K):
				return std::make_tuple(noob::keyboard::keys::K, "GLFW_KEY_K");
			case(GLFW_KEY_L):
				return std::make_tuple(noob::keyboard::keys::L, "GLFW_KEY_L");
			case(GLFW_KEY_M):
				return std::make_tuple(noob::keyboard::keys::M, "GLFW_KEY_M");
			case(GLFW_KEY_N):
				return std::make_tuple(noob::keyboard::keys::N, "GLFW_KEY_N");
			case(GLFW_KEY_O):
				return std::make_tuple(noob::keyboard::keys::O, "GLFW_KEY_O");
			case(GLFW_KEY_P):
				return std::make_tuple(noob::keyboard::keys::P, "GLFW_KEY_P");
			case(GLFW_KEY_Q):
				return std::make_tuple(noob::keyboard::keys::Q, "GLFW_KEY_Q");
			case(GLFW_KEY_R):
				return std::make_tuple(noob::keyboard::keys::R, "GLFW_KEY_R");
			case(GLFW_KEY_S):
				return std::make_tuple(noob::keyboard::keys::S, "GLFW_KEY_S");
			case(GLFW_KEY_T):
				return std::make_tuple(noob::keyboard::keys::T, "GLFW_KEY_T");
			case(GLFW_KEY_U):
				return std::make_tuple(noob::keyboard::keys::U, "GLFW_KEY_U");
			case(GLFW_KEY_V):
				return std::make_tuple(noob::keyboard::keys::V, "GLFW_KEY_V");
			case(GLFW_KEY_W):
				return std::make_tuple(noob::keyboard::keys::W, "GLFW_KEY_W");
			case(GLFW_KEY_X):
				return std::make_tuple(noob::keyboard::keys::X, "GLFW_KEY_X");
			case(GLFW_KEY_Y):
				return std::make_tuple(noob::keyboard::keys::Y, "GLFW_KEY_Y");
			case(GLFW_KEY_Z):
				return std::make_tuple(noob::keyboard::keys::Z, "GLFW_KEY_Z");
			case(GLFW_KEY_LEFT_BRACKET):
				return std::make_tuple(noob::keyboard::keys::LEFT_BRACKET, "GLFW_KEY_LEFT_BRACKET");
			case(GLFW_KEY_BACKSLASH):
				return std::make_tuple(noob::keyboard::keys::BACKSLASH, "GLFW_KEY_BACKSLASH");
			case(GLFW_KEY_RIGHT_BRACKET):
				return std::make_tuple(noob::keyboard::keys::RIGHT_BRACKET, "GLFW_KEY_RIGHT_BRACKET");
			case(GLFW_KEY_GRAVE_ACCENT):
				return std::make_tuple(noob::keyboard::keys::BACKTICK, "GLFW_KEY_GRAVE_ACCENT");
			case(GLFW_KEY_WORLD_1):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_WORLD_1");
			case(GLFW_KEY_WORLD_2):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_WORLD_2");
			case(GLFW_KEY_ESCAPE):
				return std::make_tuple(noob::keyboard::keys::ESCAPE, "GLFW_KEY_ESCAPE");
			case(GLFW_KEY_ENTER): 
				return std::make_tuple(noob::keyboard::keys::ENTER, "GLFW_KEY_ENTER");
			case(GLFW_KEY_TAB):
				return std::make_tuple(noob::keyboard::keys::TAB, "GLFW_KEY_TAB");
			case(GLFW_KEY_BACKSPACE):
				return std::make_tuple(noob::keyboard::keys::BACKSPACE, "GLFW_KEY_BACKSPACE");
			case(GLFW_KEY_INSERT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_INSERT");
			case(GLFW_KEY_DELETE):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_DELETE");
			case(GLFW_KEY_RIGHT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_RIGHT");
			case(GLFW_KEY_LEFT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_LEFT");
			case(GLFW_KEY_DOWN):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_DOWN");
			case(GLFW_KEY_UP):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_UP");
			case(GLFW_KEY_PAGE_UP): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_PAGE_UP");
			case(GLFW_KEY_PAGE_DOWN): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_PAGE_DOWN");
			case(GLFW_KEY_HOME):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_HOME");
			case(GLFW_KEY_END):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_END");
			case(GLFW_KEY_CAPS_LOCK):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_CAPS_LOCK");
			case(GLFW_KEY_SCROLL_LOCK):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_SCROLL_LOCK");
			case(GLFW_KEY_NUM_LOCK):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_NUM_LOCK");
			case(GLFW_KEY_PRINT_SCREEN):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_PRINT_SCREEN");
			case(GLFW_KEY_PAUSE):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_PAUSE");
			case(GLFW_KEY_F1):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F1");
			case(GLFW_KEY_F2):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F2");
			case(GLFW_KEY_F3):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F3");
			case(GLFW_KEY_F4):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F4");
			case(GLFW_KEY_F5):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F5");
			case(GLFW_KEY_F6):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F6");
			case(GLFW_KEY_F7): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F7");
			case(GLFW_KEY_F8): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F8");
			case(GLFW_KEY_F9):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F9");
			case(GLFW_KEY_F10): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F10");
			case(GLFW_KEY_F11):   
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F11");
			case(GLFW_KEY_F12):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F12");
			case(GLFW_KEY_F13): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F13");
			case(GLFW_KEY_F14):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F14");
			case(GLFW_KEY_F15): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F15");
			case(GLFW_KEY_F16):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F16");
			case(GLFW_KEY_F17): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F17");
			case(GLFW_KEY_F18):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F18");
			case(GLFW_KEY_F19):   
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F19");
			case(GLFW_KEY_F20):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F20");
			case(GLFW_KEY_F21): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F21");
			case(GLFW_KEY_F22):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F22");
			case(GLFW_KEY_F23):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F23");
			case(GLFW_KEY_F24):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F24");
			case(GLFW_KEY_F25):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_F25");
			case(GLFW_KEY_KP_0): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_0");
			case(GLFW_KEY_KP_1): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_1");
			case(GLFW_KEY_KP_2):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_2");
			case(GLFW_KEY_KP_3):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_3");
			case(GLFW_KEY_KP_4): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_4");
			case(GLFW_KEY_KP_5):   
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_5");
			case(GLFW_KEY_KP_6):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_6");
			case(GLFW_KEY_KP_7): 
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_7");
			case(GLFW_KEY_KP_8):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_8");
			case(GLFW_KEY_KP_9):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_9");
			case(GLFW_KEY_KP_DECIMAL):  
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_DECIMAL");
			case(GLFW_KEY_KP_DIVIDE):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_DIVIDE");
			case(GLFW_KEY_KP_MULTIPLY):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_MULTIPLY");
			case(GLFW_KEY_KP_SUBTRACT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_SUBTRACT");
			case(GLFW_KEY_KP_ADD):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_ADD");
			case(GLFW_KEY_KP_ENTER):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_ENTER");
			case(GLFW_KEY_KP_EQUAL):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_KP_EQUAL");
			case(GLFW_KEY_LEFT_SHIFT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_LEFT_SHIFT");
			case(GLFW_KEY_LEFT_CONTROL):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_LEFT_CONTROL");
			case(GLFW_KEY_LEFT_ALT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_LEFT_ALT");
			case(GLFW_KEY_LEFT_SUPER):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_LEFT_SUPER");
			case(GLFW_KEY_RIGHT_SHIFT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_RIGHT_SHIFT");
			case(GLFW_KEY_RIGHT_CONTROL):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_RIGHT_CONTROL");
			case(GLFW_KEY_RIGHT_ALT):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_RIGHT_ALT");
			case(GLFW_KEY_RIGHT_SUPER):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_RIGHT_SUPER");
			case(GLFW_KEY_MENU):
				return std::make_tuple(noob::keyboard::keys::INVALID, "GLFW_KEY_MENU");
		};
	}
}
