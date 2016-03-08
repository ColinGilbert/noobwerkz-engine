#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "VoxelWorld.hpp"

namespace noob
{
	void register_voxels(asIScriptEngine*);

	static void as_voxel_world_constructor_wrapper_basic(uint8_t* memory)
	{
		new (memory) noob::voxel_world();
	}

	static void as_voxel_world_destructor_wrapper(uint8_t* memory)
	{
		((noob::voxel_world*)memory)->~voxel_world();
	}
}
