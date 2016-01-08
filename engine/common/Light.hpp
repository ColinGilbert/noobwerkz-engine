#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	struct light 
	{
		light() : position(0.0, 100.0, 0.0, 0.0), specular(1.0, 1.0, 1.0, 0.0), diffuse(0.7, 0.7, 0.7, 0.0), ambient(0.2, 0.2, 0.2, 0.0) {}
		noob::vec4 position, specular, diffuse, ambient;
	};

	struct reflection
	{
		reflection() : specular(1.0, 1.0, 1.0, 0.0), diffuse(1.0, 1.0, 1.0, 0.0), ambient(1.0, 1.0, 1.0, 0.0), exponent(100.0) {}
		noob::vec4 specular, diffuse, ambient;
		float exponent;
	};
}
