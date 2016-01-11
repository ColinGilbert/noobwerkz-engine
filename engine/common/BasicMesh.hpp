// TODO: Add code to generate normals via assimp
// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.

#pragma once

#define OM_STATIC_BUILD 1

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/map.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Tools/Decimater/CollapseInfoT.hh>
#include <OpenMesh/Tools/Decimater/DecimaterT.hh>
#include <OpenMesh/Tools/Decimater/ModAspectRatioT.hh>
#include <OpenMesh/Tools/Decimater/ModEdgeLengthT.hh>
#include <OpenMesh/Tools/Decimater/ModHausdorffT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalDeviationT.hh>
#include <OpenMesh/Tools/Decimater/ModNormalFlippingT.hh>
#include <OpenMesh/Tools/Decimater/ModQuadricT.hh>
#include <OpenMesh/Tools/Decimater/ModProgMeshT.hh>
#include <OpenMesh/Tools/Decimater/ModIndependentSetsT.hh>
#include <OpenMesh/Tools/Decimater/ModRoundnessT.hh>
#include <OpenMesh/Tools/Subdivider/Uniform/CatmullClarkT.hh>

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

#include <Eigen/Geometry>
#include "Logger.hpp"

#define AS_CAN_USE_CPP11 1

namespace noob
{
	class basic_mesh 
	{
		friend class stage;
		friend class basic_model;
		friend class shape;
		friend class mesh_utils;
		friend class voxel_world;

		public:

			basic_mesh() : volume_calculated(false), volume(0.0) {}

			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(vertices, normals, indices, bbox_info, volume_calculated, volume);
				}


			// Proxies for AngelScript
			noob::vec3 get_vertex(unsigned int);
			noob::vec3 get_normal(unsigned int);
			noob::vec3 get_texcoord(unsigned int);
			unsigned int get_index(unsigned int);
			
			void set_vertex(unsigned int, const noob::vec3&);
			void set_normal(unsigned int, const noob::vec3&);
			void set_texcoord(unsigned int, const noob::vec3&);
			void set_index(unsigned int, unsigned int);

			double get_volume();

			noob::basic_mesh decimate(size_t num_verts) const;

			std::string save() const;
			void save(const std::string& filename) const;
			
			// Takes any file Assimp can. First mesh of file only. No bones.
			bool load_mem(const std::string&, const std::string& name = "");
			bool load_file(const std::string& filename, const std::string& name = "");

			void normalize();
			void transform(const noob::mat4& transform);
			void to_origin();
			void translate(const noob::vec3&);
			void rotate(const noob::versor&);
			void scale(const noob::vec3&);
			
			noob::bbox get_bbox() const { return bbox_info; }


		protected:
			bool load_assimp(const aiScene* scene, const std::string& name);
			TriMesh to_half_edges() const;
			void from_half_edges(TriMesh);
			void from_half_edges(PolyMesh);

			std::vector<noob::vec3> vertices;
			std::vector<noob::vec3> normals;
			std::vector<noob::vec3> texcoords;
			std::vector<uint32_t> indices;

			static constexpr aiPostProcessSteps post_process = static_cast<aiPostProcessSteps>(aiProcessPreset_TargetRealtime_Fast | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality | aiProcess_FindInstances | aiProcess_FixInfacingNormals); 
			noob::bbox bbox_info;
			bool volume_calculated;
			double volume;

	};
}
