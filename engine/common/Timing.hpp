#pragma once

#include <chrono>

#include "StringFuncs.hpp"

namespace noob
{
	static constexpr uint64_t thousand = 1000;

	static constexpr uint64_t million = 1000000;

	static constexpr uint64_t billion = 1000000000;

	typedef std::chrono::steady_clock clock;
	typedef std::chrono::time_point<noob::clock> time_point;
	typedef std::chrono::duration<clock::rep, clock::period> duration;
	typedef std::chrono::duration<double, clock::period> duration_fp;

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

	static noob::duration_fp divide_duration(const noob::duration d, uint64_t denom)
	{
		return noob::duration_fp(static_cast<double>(d.count()) / static_cast<double>(denom));
	}

	static uint64_t nanos_per_oscillation(uint64_t hz)
	{
		return static_cast<double>(noob::billion) / static_cast<double>(hz);
	}

	static std::string pretty_print_timing(const noob::duration d)
	{
		uint64_t time_in_millis = millis(d);
		if (time_in_millis > 0)
		{
			return noob::concat(noob::to_string(time_in_millis), std::string(" millis"));
		}
		else
		{
			uint64_t time_in_micros = micros(d);
			if (time_in_micros > 0)
			{
				return noob::concat(noob::to_string(time_in_micros), std::string(" micros"));
			}
			else
			{
				return noob::concat(noob::to_string(nanos(d)), std::string(" nanos"));
			}
		}
	}


	static std::string pretty_print_timing(const noob::duration_fp d)
	{
		double time_in_nanos = d.count();
		double time_in_millis = time_in_nanos / 1000000.0;
		
		if (time_in_millis > 1.0)
		{
			return noob::concat(noob::to_string(time_in_millis), std::string(" millis"));
		}
		else
		{
			double time_in_micros = time_in_nanos / 1000.0;
			if (time_in_micros > 1.0)
			{
				return noob::concat(noob::to_string(time_in_micros), std::string(" micros"));
			}
			else
			{
				return noob::concat(noob::to_string(time_in_nanos), std::string(" nanos"));
			}
		}

	}
}
