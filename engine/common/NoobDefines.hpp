#pragma once

#include <chrono>

namespace noob
{
	typedef std::chrono::steady_clock clock;
	typedef std::chrono::time_point<noob::clock> time;
	typedef std::chrono::duration<clock::rep, clock::period> duration;

	static const uint8_t DEFAULT_DRAW_LAYER = 0;
	static const uint8_t UI_TEXT_LAYER = 1;

	static const uint8_t MAX_LIGHTS = 6;
}
