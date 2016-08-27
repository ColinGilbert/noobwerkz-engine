#pragma once


#include "BasicMesh.hpp"
#include "MathFuncs.hpp"


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

		// x,y coords spun around the y-axis; first vert in vector is top-most
		// taper_one and taper_two refer to the y-distance getting tapered away from the endpoints
		// static noob::basic_mesh lathe(const std::vector<noob::vec2>& points, float taper_one, float taper_two, size_t segments);
	};
}
