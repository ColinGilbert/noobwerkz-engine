#pragma once
#include "Vec3.hpp"

// COMMON UTILITY TYPES:

enum class binary_op
{
	ADD, SUBTRACT
};

enum class csg_op
{
	UNION = 0, DIFFERENCE = 1, INTERSECTION = 2
};

enum class cardinal_axis
{
	X, Y, Z
};

struct bbox
{
	noob::vec3 get_dims() const noexcept(true)
	{
		return noob::vec3(std::fabs(min.v[0]) + std::fabs(max.v[0]), std::fabs(min.v[1]) + std::fabs(max.v[1]), std::fabs(max.v[2]) + std::fabs(max.v[2]));
	}

	noob::vec3 min, max;
};

// TODO: Find out more about the semantics of noexcept on a class declaration...
struct cubic_region noexcept(true)
{
	vec3 lower_corner, upper_corner;
};

