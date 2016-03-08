#pragma once

#include "ScaledModel.hpp"

#include <angelscript.h>
#include "AngelVector.hpp"

namespace noob
{
	void register_scaled_model(asIScriptEngine*);

	static void as_scaled_model_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::scaled_model();
	}

	static void as_scaled_model_destructor_wrapper(uint8_t* memory)
	{
		((noob::scaled_model*)memory)->~scaled_model();
	}
}
