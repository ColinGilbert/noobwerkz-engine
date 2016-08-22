#pragma once

#include "MathFuncs.hpp"


namespace noob
{
	struct reflectance
	{ 
		reflectance() noexcept(true) : specular_shine(noob::vec4(1.0, 1.0, 1.0, 32.0)), diffuse(noob::vec4(0.7, 0.7, 0.7, 0.0)), emissive(noob::vec4(0.0, 0.0, 0.0, 0.0)), rough_albedo_fresnel(noob::vec4(0.3, 0.7, 0.4, 0.0)) {}
		
		noob::vec4 specular_shine, diffuse, emissive, rough_albedo_fresnel;
	};
}
