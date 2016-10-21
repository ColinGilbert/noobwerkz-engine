// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.
#pragma once

#include <rdestl/vector.h>

#if defined(NOOB_USE_ASSIMP)
#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>
#endif

#include "MathFuncs.hpp"


// TODO: Make a bit more foolproof: IE: Functions to ensure all values get filled or something.
namespace noob
{
	struct basic_mesh 
	{
		basic_mesh() noexcept(true) : volume_calculated(false), volume(0.0) {}

		double get_volume();

		// std::string save() const;
		// void save(const std::string& filename) const;

		#if defined(NOOB_USE_ASSIMP)
		bool load_mem_assimp(const std::string&);
		bool load_file_assimp(const std::string& filename);
		bool load_assimp(const aiScene* scene);
		#endif

		// TODO: Implement and test:
		// void transform(const noob::mat4& transform);
		// void to_origin();
		// void translate(const noob::vec3&);
		// void rotate(const noob::versor&);
		// void scale(const noob::vec3&);

		noob::bbox get_bbox() const { return bbox; }

		rde::vector<noob::vec3> vertices;
		rde::vector<noob::vec3> normals;
		rde::vector<noob::vec4> colours;
		
		rde::vector<uint32_t> indices;
		// std::vector<uint8_t> flags;

		noob::bbox bbox;
		bool volume_calculated;
		double volume;
	};
}
