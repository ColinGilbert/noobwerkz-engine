#pragma once

#include <noob/math/math_funcs.hpp>
#include "Shape.hpp"

namespace noob
{
	enum class body_type : uint8_t { DYNAMIC = 0, KINEMATIC = 1, STATIC = 2 };
	
	struct body_info
	{
		noob::body_type type;
		noob::shape_handle shape;
		noob::versorf orientation;
		noob::vec3f position, linear_velocity, angular_velocity, linear_factor, angular_factor;
		float mass, friction, restitution;				
		bool ccd;
	};
}
