#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "BasicMesh.hpp"

namespace noob
{
	void register_basic_mesh(asIScriptEngine*);

	static void as_basic_mesh_constructor_wrapper(uint8_t* memory)
	{
		new(memory) noob::basic_mesh();
	}

	static void as_basic_mesh_destructor_wrapper(uint8_t* memory)
	{
		((noob::basic_mesh*)memory)->~basic_mesh();
	}
}
