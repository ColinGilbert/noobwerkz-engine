#pragma once

#include <noob/math/math_funcs.hpp>


namespace noob
{
	enum class body_type {DYNAMIC = 0, KINEMATIC = 1, STATIC = 2};
	
	struct body_info
	{
		noob::body_type type;
		float mass, friction, restitution;
		noob::vec3f position, linear_velocity, angular_velocity, linear_factor, angular_factor;
		noob::versorf orientation;
		bool ccd;
	};
}
