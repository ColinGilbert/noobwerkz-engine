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

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

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
#include "TransformHelper.hpp"
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

			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(vertices, normals, indices, bbox, volume_calculated, volume);
				}

			struct bbox_info
			{
			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(min, max, center);
				}

				noob::vec3 min, max, center;
			};

			std::vector<noob::vec3> vertices;
			std::vector<noob::vec3> normals;
			std::vector<noob::vec3> texcoords;
			std::vector<uint32_t> indices;

			double get_volume();

			void decimate(const std::string& filename, size_t num_verts) const;
			noob::basic_mesh decimate(size_t num_verts) const;


			TriMesh to_half_edges() const;

			// Returns .OFF files
			//std::tuple<size_t,const char*> save() const;
			void save(const std::string& filename) const;
			// Takes any file Assimp can. First mesh of file only. No bones.
			bool load_assimp(const std::string& filename, const std::string& name = "");
			bool load_assimp(std::tuple<size_t, const char*>, const std::string& name = "");
			bool load_assimp(const aiScene* scene, const std::string& name);
			void normalize();
			void transform(const noob::mat4& transform);
			void to_origin();
			void translate(const noob::vec3&);
			void rotate(const noob::versor&);
			void scale(const noob::vec3&);

			// Expensive. Try not calling it during actual gameplay
			std::vector<noob::basic_mesh> convex_decomposition() const;
			void calculate_texcoords();
			noob::basic_mesh::bbox_info get_bbox() const { return bbox; }

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
