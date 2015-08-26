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

// TODO: Template this class to hold different kinds of indices
// TODO: Separate the model loading from this class

#include <Eigen/Geometry>
#include "Logger.hpp"
#include "format.h"


namespace noob
{
	class basic_mesh 
	{
		public:
		
			basic_mesh() : volume_calculated(false), volume(0.0) {}

			struct bbox_info
			{
				noob::vec3 min, max, center;
			};

/*
		vertex
		{
			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(position, normal, uv, bone_names, bone_indices, bone_weights);
				}
			mesh_vertex() : position( {0.0f, 0.0f, 0.0f }), normal({ 0.0f, 0.0f, 0.0f }), uv({ 0.0f, 0.0f} ), bone_names({ "", "", "", "" }), bone_indices({0, 0, 0, 0}), bone_weights({ 0.0f, 0.0f, 0.0f, 0.0f} ) {}
			std::array<float, 3> position, normal;
			std::array<float, 2> uv;
			std::array<std::string, 4> bone_names;
			std::array<size_t, 4> bone_indices;
			std::array<float, 4> bone_weights;
		};

*/
/*
		struct mesh
		{
			template <class Archive>
				void serialize(Archive & ar)
				{
					ar(translation, scale, dimensions, rotation, name, vertices, indices, bone_names);
				}
			std::array<float, 3> translation, scale, dimensions;
			std::array<float, 4> rotation;
			std::string name;
			std::vector<mesh_vertex> vertices;
			std::vector<uint32_t> indices;
			std::vector<std::string> bone_names;
		};
*/

			std::vector<noob::vec3> vertices;
			std::vector<noob::vec3> normals;
			std::vector<uint32_t> indices;

			double get_volume();

			void decimate(const std::string& filename, size_t num_verts) const;
			noob::basic_mesh decimate(size_t num_verts) const;
			noob::basic_mesh normalize() const;
			noob::basic_mesh to_origin() const;
			TriMesh to_half_edges() const;

			// Returns .OFF files
			std::tuple<size_t,const char*> snapshot() const;
			void snapshot(const std::string& filename) const;

			// Takes any file Assimp can. First mesh of file only. No bones.
			bool load(const std::string& filename, const std::string& name = ""); //const char*, size_t);
			bool load(std::tuple<size_t, const char*>, const std::string& name = "");
			bool load(const aiScene* scene, const std::string& name);
			
			noob::basic_mesh transform(const noob::mat4& transform) const;

			// Expensive. Try not calling it too often during actual gameplay
			std::vector<noob::basic_mesh> convex_decomposition() const;

			noob::basic_mesh::bbox_info get_bbox() const { return bbox; }

			// TODO: Fix
			//static noob::basic_mesh csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op);
			static noob::basic_mesh cone(float radius, float height, size_t segments = 0);
			static noob::basic_mesh cube(float width, float height, float depth, size_t subdivides = 0);
			static noob::basic_mesh cylinder(float radius, float height, size_t segments = 0);
			static noob::basic_mesh sphere(float radius);
			// static noob::basic_mesh bone();

		protected:
			bbox_info bbox;
			bool volume_calculated;
			double volume;
	};
}
