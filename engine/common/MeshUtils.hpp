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
			static noob::mesh_3d box(float width, float height, float depth); // TODO: BROKEN: Fix when adding box-modeling
			// static noob::mesh_3d cone(float radius, float height, uint32_t segments);
			static noob::mesh_3d cylinder(float radius, float height, uint32_t segments, bool narrowed_end = false, float narrow_to = 0.0);
			static noob::mesh_3d cone(float radius, float height, uint32_t segments);
			static noob::mesh_3d hull(const std::vector<noob::vec3f>& points); // TODO: Investigate scaling (or overall correctness) issues
			// TODO: Test
			static noob::mesh_3d circle(float radius, uint32_t segments);
	};
}
