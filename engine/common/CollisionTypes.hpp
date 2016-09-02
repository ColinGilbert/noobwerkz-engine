#pragma once

#include <LinearMath/btConvexHull.h>

#include "Vec3.hpp"

namespace noob
{
	// Would prefer uint32_t, but Bullet defines it as an int
	enum collision_type
	{
		NOTHING = 0,
		SCENERY = 1 << 1,
		CHARACTER = 1 << 2,
		POWERUP = 1 << 3,
		PARTICLE = 1 << 4,
		TEAM_A = 1 << 5,
		TEAM_B = 1 << 6,
		TEAM_C = 1 << 7,
		TEAM_D = 1 << 8
	};
}
