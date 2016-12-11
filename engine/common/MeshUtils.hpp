#pragma once

// External libs
#include <noob/math/math_funcs.hpp>

// Project-local
#include "Mesh3D.hpp"


namespace noob
{
	class mesh_utils
	{
		public:
			static noob::mesh_3d sphere(float radius, uint32_t details);
			static noob::mesh_3d box(float width, float height, float depth);
			// static noob::mesh_3d cone(float radius, float height, uint32_t segments);
			// static noob::mesh_3d cylinder(float radius, float height, uint32_t segments);
			static noob::mesh_3d hull(const std::vector<noob::vec3>& points);
			// TODO: Test
			static noob::mesh_3d circle(float radius, uint32_t segments);
	};
}
