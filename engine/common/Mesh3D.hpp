// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.
#pragma once

#include <rdestl/vector.h>
#include <noob/math/math_funcs.hpp>
#include <noob/component/component.hpp>

#if defined(NOOB_USE_ASSIMP)
#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#endif

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

			double get_volume();

			// std::string save() const;
			// void save(const std::string& filename) const;
			void calculate_dims() noexcept(true);

#if defined(NOOB_USE_ASSIMP)
			bool load_mem_assimp(const std::string&);
			bool load_file_assimp(const std::string& filename);
			bool load_assimp(const aiScene* scene);
#endif

			// TODO: Implement and test:
			// void transform(const noob::mat4f& transform);
			// void to_origin();
			// void translate(const noob::vec3f&);
			// void rotate(const noob::versorf&);
			// void scale(const noob::vec3f&);

			rde::vector<noob::mesh_3d::vert> vertices;

			rde::vector<uint32_t> indices;
			// std::vector<uint8_t> flags;

			noob::bbox_type<float> bbox;
			bool volume_calculated = false;
			double volume = 0.0;
	};
}
