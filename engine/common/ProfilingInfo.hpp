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
		noob::time::duration stage_physics_time;
		noob::time::duration stage_draw_time;
/*
		std::string to_string() const
		{
			fmt::MemoryWriter ww;
			ww << "total(" << noob::micros(total_time) << "), physics(" << noob::micros(stage_physics_time) << "), draw(" << noob::micros(stage_draw_time) << ")";
			return ww.str();
		}
*/
	};
}
