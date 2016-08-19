#pragma once

#include <string>

#include "format.h"

#include "NoobDefines.hpp"
#include "Timing.hpp"

namespace noob
{
	struct profiler_snap
	{
		noob::time::duration total_time;
		// noob::time::duration app_draw_duration;
		noob::time::duration stage_ai_duration;
		noob::time::duration stage_physics_duration;
		noob::time::duration stage_draw_duration;
		
		std::string to_string() const
		{
			fmt::MemoryWriter ww;
			ww << "total(" << noob::pretty_print_timing(total_time) << "), ai(" << noob::pretty_print_timing(stage_ai_duration) << ", physics(" << noob::pretty_print_timing(stage_physics_duration) << "), nanos(" << noob::pretty_print_timing(stage_draw_duration) << ")";
			return ww.str();
		}
	};
}
