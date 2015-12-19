#pragma once

#include <vector>
#include <algorithm>
#include <math.h>

struct csgjs_vector
{
	float x, y, z;

	csgjs_vector() : x(0.0f), y(0.0f), z(0.0f) {}
	explicit csgjs_vector(float x, float y, float z) : x(x), y(y), z(z) {}
};

struct csgjs_vertex
{
	csgjs_vector pos;
	csgjs_vector normal;
	csgjs_vector uv;
};

struct csgjs_model
{
	std::vector<csgjs_vertex> vertices;
	std::vector<int> indices;
};

// public interface - not super efficient, if you use multiple CSG operations you should
// use BSP trees and convert them into model only once. Another optimization trick is
// replacing csgjs_model with your own class.

csgjs_model csgjs_union(const csgjs_model & a, const csgjs_model & b);
csgjs_model csgjs_intersection(const csgjs_model & a, const csgjs_model & b);
csgjs_model csgjs_difference(const csgjs_model & a, const csgjs_model & b);

// IMPLEMENTATION BELOW ---------------------------------------------------------------------------


