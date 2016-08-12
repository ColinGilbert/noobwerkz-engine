#pragma once

#include "ComponentDefines.hpp"

namespace noob
{
	struct scaled_model
	{
		scaled_model() : scales(noob::vec3(1.0, 1.0, 1.0)) {}
		noob::model_handle model_h;
		// noob::reflectance_handle reflect_h;
		noob::vec3 scales;
	};
}
