#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "ShadingVariant.hpp"

namespace noob
{
	void register_shading_variant(asIScriptEngine* script_engine);

	static void as_shader_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::shader();
	}

	static void as_shader_destructor_wrapper(uint8_t* memory)
	{
		((noob::shader*)memory)->~shader();
	}

	static void as_surface_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::surface();
	}

	static void as_surface_destructor_wrapper(uint8_t* memory)
	{
		((noob::surface*)memory)->~surface();
	}
}
