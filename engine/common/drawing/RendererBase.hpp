#pragma once

#include "NoobDefines.hpp"
#include "Light.hpp"
#include "Reflectance.hpp"
#include "Graphics.hpp"

namespace noob
{
	struct renderer_base
	{
		bool program_valid;
		noob::graphics::shader shader;
	};
}
