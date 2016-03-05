#pragma once

#include "Light.hpp"
#include "Reflectance.hpp"

namespace noob
{
	class renderer
	{
		protected:
			noob::graphics::shader shader;
			bool program_valid;
	};
}
