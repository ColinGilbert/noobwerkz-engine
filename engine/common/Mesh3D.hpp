// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.
#pragma once

#include <rdestl/vector.h>
#include <noob/math/math_funcs.hpp>

#if defined(NOOB_USE_ASSIMP)
#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#endif



// TODO: Make a bit more foolproof: IE: Functions to ensure all values get filled or something.
namespace noob
{
	struct mesh_3d
	{
		mesh_3d() noexcept(true) : volume_calculated(false), volume(0.0) {}

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

		rde::vector<noob::vec3f> vertices;
		rde::vector<noob::vec3f> normals;
		rde::vector<noob::vec4f> texcoords;
		rde::vector<noob::vec4f> colours;
		rde::vector<uint32_t> indices;
		// std::vector<uint8_t> flags;

		noob::bbox_type<float> bbox;
		bool volume_calculated;
		double volume;
	};
}
