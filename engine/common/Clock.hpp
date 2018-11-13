#pragma once

#include <chrono>

namespace noob
{
	typedef std::chrono::steady_clock clock;
	typedef std::chrono::time_point<noob::clock> time;
	typedef std::chrono::duration<noob::clock::rep, noob::clock::period> duration;
	typedef std::chrono::duration<double, noob::clock::period> duration_fp;
}
