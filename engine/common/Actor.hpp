#pragma once

#include "ComponentDefines.hpp"
#include "Graph.hpp"

namespace noob
{
	struct basic_actor
	{
		noob::shader_type shading_type;
		uint32_t shading_index;
		noob::body_handle bod;
	};
}
