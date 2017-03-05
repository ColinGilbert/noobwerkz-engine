#pragma once

#include <string>

#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

#include "Results.hpp"
#include "Mesh3D.hpp"

namespace noob
{
	struct assimp
	{
		static noob::results<noob::mesh_3d> load_mem(const std::string&);
		static noob::results<noob::mesh_3d> load_file(const std::string& filename);
		static noob::results<noob::mesh_3d> load(const aiScene* scene);
	};
}
