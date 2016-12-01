#pragma once

#include <string>

#include "Timing.hpp"

namespace noob
{
	struct profiler_snap
	{
		noob::duration total_time;
		// noob::duration app_draw_duration;
		noob::duration stage_ai_duration;
		noob::duration stage_physics_duration;
		noob::duration stage_draw_duration;
		noob::duration sound_render_duration;
		size_t num_sound_callbacks;
		
		std::string to_string() const
		{
			return noob::concat("total(",  noob::to_string(total_time),  "), ai(", noob::to_string(stage_ai_duration), ", physics(", noob::to_string(stage_physics_duration), "), nanos(", noob::to_string(stage_draw_duration), ")");
		}
	};
}
