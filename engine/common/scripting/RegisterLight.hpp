#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "Light.hpp"


namespace noob
{
	void register_light(asIScriptEngine* script_engine);

	static void as_light_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::light();
	}

	static void as_light_destructor_wrapper(uint8_t* memory)
	{
		((noob::light*)memory)->~light();
	}
}
