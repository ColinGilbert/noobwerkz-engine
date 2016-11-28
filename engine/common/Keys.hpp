#pragma once

namespace noob
{
	struct keyboard
	{
		enum class keys { A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, MINUS, EQUAL, BACKSPACE, PERIOD, COMMA, SLASH, TAB, RETURN, UP, DOWN, LEFT, RIGHT, NUM_1, NUM_2, NUM_3, NUM_4, NUM_5, NUM_6, NUM_7, NUM_8, NUM_9, NUM_0, NUM_DIV, NUM_MULT, NUM_MINUS, NUM_PLUS, NUM_ENTER, PAGEUP, PAGEDOWN, ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, INSERT, DELETE, HOME, END, ALT, CTRL, SHIFT };

		static std::string to_string(noob::keyboard::keys k)
		{
			switch(k)
			{
				case (noob::keyboard::keys::A):
					return "A";
				case (noob::keyboard::keys::B):
					return "B";
				case (noob::keyboard::keys::C):
					return "C";
				case (noob::keyboard::keys::D):
					return "D";
				case (noob::keyboard::keys::E):
					return "E";
				case (noob::keyboard::keys::F):
					return "F";
				case (noob::keyboard::keys::G):
					return "G";
				case (noob::keyboard::keys::H):
					return "H";
				case (noob::keyboard::keys::I):
					return "I";
				case (noob::keyboard::keys::J):
					return "J";
				case (noob::keyboard::keys::K):
					return "K";
				case (noob::keyboard::keys::L):
					return "L";
				case (noob::keyboard::keys::M):
					return "M";
				case (noob::keyboard::keys::N):
					return "N";
				case (noob::keyboard::keys::O):
					return "O";
				case (noob::keyboard::keys::P):
					return "P";
				case (noob::keyboard::keys::Q):
					return "Q";
				case (noob::keyboard::keys::R):
					return "R";
				case (noob::keyboard::keys::S):
					return "S";
				case (noob::keyboard::keys::T):
					return "T";
				case (noob::keyboard::keys::U):
					return "U";
				case (noob::keyboard::keys::V):
					return "V";
				case (noob::keyboard::keys::W):
					return "W";
				case (noob::keyboard::keys::X):
					return "X";
				case (noob::keyboard::keys::Y):
					return "Y";
				case (noob::keyboard::keys::Z):
					return "Z";
				case (noob::keyboard::keys::_1):
					return "1";
				case (noob::keyboard::keys::_2):
					return "2";
				case (noob::keyboard::keys::_3):
					return "3";
				case (noob::keyboard::keys::_4):
					return "4";
				case (noob::keyboard::keys::_5):
					return "5";
				case (noob::keyboard::keys::_6):
					return "6";
				case (noob::keyboard::keys::_7):
					return "7";
				case (noob::keyboard::keys::_8):
					return "8";
				case (noob::keyboard::keys::_9):
					return "9";
				case (noob::keyboard::keys::_0):
					return "0";
				case (noob::keyboard::keys::MINUS):
					return "-";
				case (noob::keyboard::keys::EQUAL):
					return "=";
				case (noob::keyboard::keys::BACKSPACE):
					return "BACKSPACE";
				case (noob::keyboard::keys::PERIOD):
					return ".";
				case (noob::keyboard::keys::COMMA):
					return ",";
				case (noob::keyboard::keys::SLASH):
					return "/";
				case (noob::keyboard::keys::TAB):
					return "TAB";
				case (noob::keyboard::keys::RETURN):
					return "RETURN";
				case (noob::keyboard::keys::UP):
					return "UP";
				case (noob::keyboard::keys::DOWN):
					return "DOWN";
				case (noob::keyboard::keys::LEFT):
					return "LEFT";
				case (noob::keyboard::keys::RIGHT):
					return "RIGHT";
				case (noob::keyboard::keys::NUM_1):
					return "NUMPAD 1";
				case (noob::keyboard::keys::NUM_2):
					return "NUMPAD 2";
				case (noob::keyboard::keys::NUM_3):
					return "NUMPAD 3";
				case (noob::keyboard::keys::NUM_4):
					return "NUMPAD 4";
				case (noob::keyboard::keys::NUM_5):
					return "NUMPAD 5";
				case (noob::keyboard::keys::NUM_6):
					return "NUMPAD 6";
				case (noob::keyboard::keys::NUM_7):
					return "NUMPAD 7";
				case (noob::keyboard::keys::NUM_8):
					return "NUMPAD 8";
				case (noob::keyboard::keys::NUM_9):
					return "NUMPAD 9";
				case (noob::keyboard::keys::NUM_0):
					return "NUMPAD 0";
				case (noob::keyboard::keys::NUM_DIV):
					return "NUMPAD /";
				case (noob::keyboard::keys::NUM_MULT):
					return "NUMPAD *";
				case (noob::keyboard::keys::NUM_MINUS):
					return "NUMPAD -";
				case (noob::keyboard::keys::NUM_PLUS):
					return "NUMPAD +";
				case (noob::keyboard::keys::NUM_ENTER):
					return "NUMPAD ENTER";
				case (noob::keyboard::keys::PAGEUP):
					return "PAGEUP";
				case (noob::keyboard::keys::PAGEDOWN):
					return "PAGEDOWN";
				case (noob::keyboard::keys::ESC):
					return "ESC";
				case (noob::keyboard::keys::F1):
					return "F1";
				case (noob::keyboard::keys::F2):
					return "F2";
				case (noob::keyboard::keys::F3):
					return "F3";
				case (noob::keyboard::keys::F4):
					return "F4";
				case (noob::keyboard::keys::F5):
					return "F5";
				case (noob::keyboard::keys::F6):
					return "F6";
				case (noob::keyboard::keys::F7):
					return "F7";
				case (noob::keyboard::keys::F8):
					return "F8";
				case (noob::keyboard::keys::F9):
					return "F9";
				case (noob::keyboard::keys::F10):
					return "F10";
				case (noob::keyboard::keys::F11):
					return "F11";
				case (noob::keyboard::keys::F12):
					return "F12";
				case (noob::keyboard::keys::INSERT):
					return "INSERT";
				case (noob::keyboard::keys::DELETE):
					return "DELETE";
				case (noob::keyboard::keys::HOME):
					return "HOME";
				case (noob::keyboard::keys::END):
					return "END";
				case (noob::keyboard::keys::ALT):
					return "ALT";
				case (noob::keyboard::keys::CTRL):
					return "CTRL";
				case (noob::keyboard::keys::SHIFT):
					return "SHIFT";
				default:
					return "UNKNOWN KEY";
			}
		}
	};
}
