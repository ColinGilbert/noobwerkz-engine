#pragma once

#include "NoobDefines.hpp"
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
