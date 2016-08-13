#pragma once

#include "NoobDefines.hpp"

namespace noob
{
	static uint64_t millis(const noob::duration d)
	{
		return (std::chrono::duration_cast<std::chrono::milliseconds>(d)).count();
	}

	static uint64_t micros(const noob::duration d)
	{
		return (std::chrono::duration_cast<std::chrono::microseconds>(d)).count();
	}

	static uint64_t nanos(const noob::duration d)
	{
		return (std::chrono::duration_cast<std::chrono::nanoseconds>(d)).count();
	}
}
