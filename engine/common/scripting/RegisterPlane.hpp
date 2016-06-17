#pragma once

#include "Plane.hpp"
#include <angelscript.h>
#include "AngelVector.hpp"

namespace noob
{
	void register_plane(asIScriptEngine*);

	static void as_plane_constructor_wrapper_basic(uint8_t* memory)
	{
		new(memory) noob::plane();
	}

	static void as_plane_destructor_wrapper(uint8_t* memory)
	{
		((noob::plane*)memory)->~plane();
	}
}
