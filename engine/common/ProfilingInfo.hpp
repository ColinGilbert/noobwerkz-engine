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
			return noob::concat("total(",  noob::to_string(total_time),  "), ai(", noob::to_string(stage_ai_duration), ", physics(", noob::to_string(stage_physics_duration), "), nanos(", noob::to_string(stage_draw_duration), ")");
		}
	};
}
