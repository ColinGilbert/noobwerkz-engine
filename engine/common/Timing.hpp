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

	static noob::duration_fp divide_duration(const noob::duration d, uint64_t denom)
	{
		return noob::duration_fp(static_cast<double>(d.count()) / static_cast<double>(denom));
	}

	static std::string pretty_print_timing(const noob::duration d)
	{
		fmt::MemoryWriter ww;
		uint64_t time_in_millis = millis(d);
		if (time_in_millis > 0)
		{
			ww << time_in_millis << " millis";
		}
		else
		{
			uint64_t time_in_micros = micros(d);
			if (time_in_micros > 0)
			{
				ww << time_in_micros << " micros";
			}
			else
			{
				ww << nanos(d) << " nanos";
			}
		}

		return ww.str();
	}


	static std::string pretty_print_timing(const noob::duration_fp d)
	{
		fmt::MemoryWriter ww;
		double time_in_nanos = d.count();
		double time_in_millis = time_in_nanos / 1000000.0;
		if (time_in_millis > 1.0)
		{
			ww << time_in_millis << " millis";
		}
		else
		{
			double time_in_micros = time_in_nanos / 1000.0;
			if (time_in_micros > 1.0)
			{
				ww << time_in_micros << " micros";
			}
			else
			{
				ww << time_in_nanos << " nanos";
			}
		}

		return ww.str();
	}


}
