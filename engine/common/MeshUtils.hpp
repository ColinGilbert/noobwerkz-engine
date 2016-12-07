#pragma once

// External libs
#include <noob/math/math_funcs.hpp>

// Project-local
#include "BasicMesh.hpp"


namespace noob
{
	class mesh_utils
	{
		public:
			static noob::basic_mesh sphere(float radius, uint32_t details);
			static noob::basic_mesh box(float width, float height, float depth);
			// static noob::basic_mesh cone(float radius, float height, uint32_t segments);
			// static noob::basic_mesh cylinder(float radius, float height, uint32_t segments);
			static noob::basic_mesh hull(const std::vector<noob::vec3>& points);
			// TODO: Test
			static noob::basic_mesh circle(float radius, uint32_t segments);
	};
}
