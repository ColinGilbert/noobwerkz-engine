#pragma once

#include "MathFuncs.hpp"

namespace noob
{
// uniform vec4 emissive_coeff;
// uniform vec4 diffuse_coeff;
// uniform vec4 ambient_coeff;
// uniform vec4 specular_coeff;
// uniform vec4 light_direction;
// uniform vec4 light_pos;
// uniform vec4 light_colour;
// uniform vec4 global_ambient;

// uniform float specular_power;
// uniform float spot_power;


	struct directional_light 
	{
		noob::vec3 direction, colour, emission, ambient, diffuse, specular;
		float specular_power;
	};

	struct point_light 
	{
		noob::vec3 position, colour, emission, ambient, diffuse, specular;
		float specular_power;
	};

	struct spotlight 
	{
		noob::vec3 direction, position, colour, emission, ambient, diffuse, specular;
		float specular_power, spot_power;
	};

/*
	struct light 
	{
		light() : position(0.0, 100.0, 0.0, 0.0), specular(1.0, 1.0, 1.0, 0.0), diffuse(0.7, 0.7, 0.7, 0.0), ambient(0.2, 0.2, 0.2, 0.0) {}
		// noob::vec4 position, specular, diffuse, ambient;
		// emission, ambient, diffuse, specular.
	};

	struct reflection
	{
		reflection() : specular(1.0, 1.0, 1.0, 0.0), diffuse(1.0, 1.0, 1.0, 0.0), ambient(1.0, 1.0, 1.0, 0.0), exponent(100.0) {}
		noob::vec4 specular, diffuse, ambient;
		float exponent;
	};
*/
}
