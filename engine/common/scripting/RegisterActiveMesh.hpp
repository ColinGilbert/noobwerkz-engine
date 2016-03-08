#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "ActiveMesh.hpp"


namespace noob
{
	void register_active_mesh(asIScriptEngine*);

	static void as_active_mesh_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::active_mesh();
	}

	static void as_active_mesh_destructor_wrapper(uint8_t* memory)
	{
		((noob::active_mesh*)memory)->~active_mesh();
	}
}
