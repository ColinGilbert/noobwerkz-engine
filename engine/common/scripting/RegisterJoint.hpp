#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"
#include "Joint.hpp"

namespace noob
{
	void register_joint(asIScriptEngine* script_engine);

	static void as_joint_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::joint();
	}

	static void as_joint_destructor_wrapper(uint8_t* memory)
	{
		((noob::joint*)memory)->~joint();
	}

}
