#pragma once

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "Vec3.hpp"

namespace noob
{
	enum class collision_type
	{
		NOTHING = 0,
		TERRAIN = 1 << 1,
		CHARACTER = 1 << 2,
		TEAM_A = 1 << 3,
		TEAM_B = 1 << 4,
		POWERUP = 1 << 5,
	};

	enum class pos_type
	{
		GHOST = 0, PHYSICAL = 1, ANIM = 2
	};

	struct body_variant
	{
		pos_type type;
		uint32_t index;
	};

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
