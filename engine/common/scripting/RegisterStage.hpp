#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "Stage.hpp"

namespace noob
{
	void register_stage(asIScriptEngine* script_engine);

	static void as_stage_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::stage();
	}

	static void as_stage_destructor_wrapper(uint8_t* memory)
	{
		((noob::stage*)memory)->~stage();
	}
}
