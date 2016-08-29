#pragma once

#include "BasicMesh.hpp"
#include "MathFuncs.hpp"

#include "voro++.hh"

namespace noob
{
	class mesh_utils
	{
		public:
		static noob::basic_mesh sphere(float radius);
		static noob::basic_mesh box(float width, float height, float depth);
		static noob::basic_mesh cone(float radius, float height);
		static noob::basic_mesh cylinder(float radius, float height, size_t segments = 0);
		static noob::basic_mesh hull(const std::vector<noob::vec3>& points);
		static noob::basic_mesh voro_to_mesh(voro::voronoicell*);
	};
}
