#pragma once
#pragma once

#include "MathFuncs.hpp"
namespace noob
{
struct plane
{
	noob::vec3 normal;
	float w;

	through(noob::vec3& a, const noob::vec3& b, const noob::vec3& c)
	{
		normal = noob::normalize(noob::cross(b - a, c - a));
		w = noob::dot(normal, a);
	}
	
	bool ok() const
	{
		return noob::length(normal) > 0.0f; 
	}

	void flip()
	{
		normal = noob::negate(normal);
		w *= -1.0;
	}

	// void split_polygon(const csgjs_polygon & polygon, std::vector<csgjs_polygon> & coplanarFront, std::vector<csgjs_polygon> & coplanarBack, std::vector<csgjs_polygon> & front, std::vector<csgjs_polygon> & back) const;
};
}
