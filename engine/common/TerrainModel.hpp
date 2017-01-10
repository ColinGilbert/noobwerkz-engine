// Terrain is dynamically generated from scenery and uploaded whenever needed. Our 

#pragma once

#include "Model.hpp"

namespace noob
{
	struct terrain_model : public model_buffered
	{
		static constexpr uint32_t stride = sizeof(noob::vec4f) * 3;
	};
}
