#pragma once

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "Vec3.hpp"

namespace noob
{
	enum class shader_type
	{       
		BASIC = 0,
		TRIPLANAR = 1
	};

	struct drawing_info
	{
		noob::shader_type type;
		uint32_t shader_index;
		uint32_t reflect;
	};

	typedef rde::fixed_array<noob::drawing_info, 32> character_colour_scheme;
	typedef rde::vector<noob::drawing_info> boss_colour_scheme;

	typedef noob::component<noob::character_colour_scheme> character_shading_holder;
	typedef noob::component_dynamic<noob::boss_colour_scheme> boss_shading_holder;

	typedef handle<noob::character_colour_scheme> character_shading_handle;
	typedef handle<noob::boss_colour_scheme> boss_shading_handle;
	
	struct contact_point
	{
		size_t handle;
		noob::vec3 pos_a, pos_b, normal_on_b;
	};

	struct ghost_intersection_results
	{
		noob::ghost_handle ghost;
		std::vector<noob::body_handle> bodies;
		std::vector<noob::ghost_handle> ghosts;
	};
}
