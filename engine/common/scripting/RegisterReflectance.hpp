#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "Reflectance.hpp"

namespace noob
{
	void register_reflectance(asIScriptEngine*);

	static void as_reflectance_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::reflectance();
	}

	static void as_reflectance_destructor_wrapper(uint8_t* memory)
	{
		((noob::reflectance*)memory)->~reflectance();
	}
}	
