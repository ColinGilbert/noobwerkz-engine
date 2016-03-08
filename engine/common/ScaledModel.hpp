#pragma once

#include "ComponentDefines.hpp"

namespace noob
{
	struct scaled_model
	{
		scaled_model() : scales(noob::vec3(1.0, 1.0, 1.0)) {}
		noob::basic_models_holder::handle model_h;
		noob::reflectances_holder::handle reflect_h;
		noob::vec3 scales;
	};
}
