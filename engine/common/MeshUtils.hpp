#pragma once


#include "BasicMesh.hpp"
#include "MathFuncs.hpp"

//#define CSGJS_HEADER_ONLY
#include <csgjs.hpp>

namespace noob
{
	class mesh_utils
	{
		public:
		static noob::basic_mesh cone(float radius, float height, size_t segments = 0);
		static noob::basic_mesh cube(float width, float height, float depth, size_t subdivides = 0);
		static noob::basic_mesh cylinder(float radius, float height, size_t segments = 0);
		static noob::basic_mesh swept_sphere(float radius, size_t x_segment = 0, size_t y_segment = 0);
		static noob::basic_mesh catmull_sphere(float radius);
		static noob::basic_mesh hull(const std::vector<noob::vec3>& points);
		static noob::basic_mesh csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op);


		protected:
		static csgjs_model get_csg_model(const noob::basic_mesh& m);
	};
}
