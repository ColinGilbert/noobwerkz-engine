// TODO: Add code to generate normals via assimp
// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.

#pragma once

#define OM_STATIC_BUILD 1
/*
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>
*/

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;
typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;

#include "MathFuncs.hpp"

// TODO: Template this class to hold different indices

#include <Eigen/Geometry>
#include "Logger.hpp"
#include "format.h"


namespace noob
{
	class mesh 
	{
		public:
			struct bbox_info
			{
				noob::vec3 min, max, bbox_center;//, centroid;
			};

			std::vector<noob::vec3> vertices;
			std::vector<noob::vec3> normals;
			std::vector<uint32_t> indices;

			void decimate(const std::string& filename, size_t num_verts) const;
			noob::mesh decimate(size_t num_verts) const;
			noob::mesh normalize() const;
			noob::mesh to_origin() const;
			TriMesh to_half_edges() const;

			// Returns .OFF files
			std::tuple<size_t,const char*> snapshot() const;
			void snapshot(const std::string& filename) const;

			// Takes any file Assimp can. First mesh of file only. No bones.
			bool load(const std::string& filename, const std::string& name = ""); //const char*, size_t);
			bool load(std::tuple<size_t, const char*>, const std::string& name = "");
			bool load(const aiScene* scene, const std::string& name);
			
			noob::mesh transform(const noob::mat4& transform) const;

			// Expensive. Try not calling it too often during actual gameplay
			std::vector<noob::mesh> convex_decomposition() const;

			noob::mesh::bbox_info get_bbox() const { return info; }

			// TODO: Fix
			//static noob::mesh csg(const noob::mesh& a, const noob::mesh& b, const noob::csg_op op);
			static noob::mesh cone(float radius, float height, size_t subdivides = 0);
			static noob::mesh cube(float width, float height, float depth, size_t subdivides = 0);
			static noob::mesh cylinder(float radius, float height, size_t subdivides = 0);
			static noob::mesh sphere(float radius);

		protected:
			bbox_info info;
	};
}
