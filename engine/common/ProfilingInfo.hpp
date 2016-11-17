#pragma once

#include <string>

#include "Timing.hpp"

namespace noob
{
	struct profiler_snap
	{
		noob::time_point::duration total_time;
		// noob::time_point::duration app_draw_duration;
		noob::time_point::duration stage_ai_duration;
		noob::time_point::duration stage_physics_duration;
		noob::time_point::duration stage_draw_duration;
		
		std::string to_string() const
		{
			return noob::concat("total(",  noob::pretty_print_timing(total_time),  "), ai(", noob::pretty_print_timing(stage_ai_duration), ", physics(", noob::pretty_print_timing(stage_physics_duration), "), nanos(", noob::pretty_print_timing(stage_draw_duration), ")");
		}
	};
}
