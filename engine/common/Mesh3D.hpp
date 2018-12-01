// TODO: Replace hard-coded box, cylinder, cone functions with more generic extruded/swept shape algorithm. Low priority.
#pragma once

#include <rdestl/vector.h>
#include <noob/math/math_funcs.hpp>
#include <noob/component/component.hpp>

namespace noob
{
	class mesh_3d
	{
		public:

			struct vert
			{
				noob::vec3f position;
				noob::vec3f normal;
				noob::vec3f texcoords; // Allows two sets of 2D texcoords for multitexturing
				noob::vec4f colour; //Needed?
			};

			float get_volume();

			void calculate_dims() noexcept(true);


			// TODO: Implement and test:
			// void transform(const noob::mat4f& transform);
			// void to_origin();
			// void translate(const noob::vec3f&);
			// void rotate(const noob::versorf&);
			// void scale(const noob::vec3f&);

			rde::vector<noob::mesh_3d::vert> vertices;
			// TODO: Increase type-safety by replacing with triangles.
			rde::vector<uint32_t> indices;
			// std::vector<uint8_t> flags;

			noob::bbox_type<float> bbox;
			bool volume_calculated = false;
			double volume = 0.0;
	};
}
