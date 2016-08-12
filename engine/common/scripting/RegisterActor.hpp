#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "Actor.hpp"

namespace noob
{
	void register_actor(asIScriptEngine* script_engine);

	static void as_actor_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::actor();
	}

	static void as_actor_destructor_wrapper(uint8_t* memory)
	{
		((noob::actor*)memory)->~actor();
	}

	static void as_actor_blueprints_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::actor_blueprints();
	}

	static void as_actor_blueprints_destructor_wrapper(uint8_t* memory)
	{
		((noob::actor_blueprints*)memory)->~actor_blueprints();
	}

}
