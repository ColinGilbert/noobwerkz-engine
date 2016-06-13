#pragma once


#include "BasicMesh.hpp"
#include "MathFuncs.hpp"


// #include <Newton.h>
//#define CSGJS_HEADER_ONLY
// #include <csgjs.hpp>

namespace noob
{
	class mesh_utils
	{
		public:
		static noob::basic_mesh sphere(float radius);
		static noob::basic_mesh cone(float radius, float height, size_t segments = 0);
		static noob::basic_mesh box(float width, float height, float depth, size_t subdivides = 0);
		static noob::basic_mesh cylinder(float radius, float height, size_t segments = 0);
		static noob::basic_mesh swept_sphere(float radius, size_t x_segment = 0, size_t y_segment = 0);
		static noob::basic_mesh catmull_sphere(float radius);
		static noob::basic_mesh hull(const std::vector<noob::vec3>& points);

		// x,y coords spun around the y-axis; clip_one and clip_two refer to the y-distance getting tapered away from the endpoints.
		static noob::basic_mesh lathe(const std::vector<noob::vec2>& points, float clip_one, float clip_two);
	};
}
