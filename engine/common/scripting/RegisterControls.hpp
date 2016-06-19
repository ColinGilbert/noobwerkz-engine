#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"
#include "Controls.hpp"

namespace noob
{
	void register_controls(asIScriptEngine* script_engine);

	static void as_controls_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::controls();
	}

	static void as_controls_destructor_wrapper(uint8_t* memory)
	{
		((noob::controls*)memory)->~controls();
	}

}
