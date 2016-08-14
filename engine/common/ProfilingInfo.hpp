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
		// noob::time::duration app_draw_time;
		noob::time::duration stage_ai_time;
		noob::time::duration stage_physics_time;
		noob::time::duration stage_draw_time;
		
		std::string to_string() const
		{
			fmt::MemoryWriter ww;
			ww << "total(" << noob::pretty_print_timing(total_time) << "), ai(" << noob::pretty_print_timing(stage_physics_time) << ", physics(" << noob::pretty_print_timing(stage_physics_time) << "), nanos(" << noob::pretty_print_timing(stage_draw_time) << ")";
			return ww.str();
		}
	};
}
