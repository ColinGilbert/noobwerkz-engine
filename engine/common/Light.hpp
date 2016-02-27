#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	struct light 
	{ 	
		enum light_type {DIRECTIONAL = 0, POINT = 1, SPOT = 2};
		
		light() : position(noob::vec3(0.0, 100.0, 0.0)), direction(noob::vec3(-0.1, -1.0, -1.0)), colour(noob::vec3(1.0, 1.0, 1.0)), emissive(noob::vec3(0.0, 0.0, 0.0)), ambient(noob::vec3(0.4, 0.4, 0.4)), diffuse(noob::vec3(1.0, 1.0, 1.0)), specular(noob::vec3(0.6, 0.6, 0.6)), specular_power(50.0), spot_power(10.0), type(noob::light::light_type::POINT) {}

		noob::vec3 position, direction, colour, emissive, ambient, diffuse, specular;
		float specular_power, spot_power;
		light_type type;
	};
}
