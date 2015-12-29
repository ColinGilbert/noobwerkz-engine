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

namespace noob
{
	class basic_mesh 
	{
		public:

			basic_mesh() : volume_calculated(false), volume(0.0) {}

			template <class Archive>
				void serialize( Archive & ar )
				{
					ar(vertices, normals, indices, bbox_info, volume_calculated, volume);
				}

			struct bbox
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
			//noob::basic_mesh decimate(size_t num_verts) const;

			std::string save() const;
			void save(const std::string& filename) const;
			
			// Takes any file Assimp can. First mesh of file only. No bones.
			bool load_mem(const std::string& file, const std::string& name = "");
			bool load_file(const std::string& filename, const std::string& name = "");
			bool load_assimp(const aiScene* scene, const std::string& name);


			void normalize();
			void transform(const noob::mat4& transform);
			void to_origin();
			void translate(const noob::vec3&);
			void rotate(const noob::versor&);
			void scale(const noob::vec3&);
			
			noob::basic_mesh::bbox get_bbox() const { return bbox_info; }

			TriMesh to_half_edges() const;
			void from_half_edges(TriMesh);
			void from_half_edges(PolyMesh);

			// size_t references;

		protected:
			static constexpr aiPostProcessSteps post_process = static_cast<aiPostProcessSteps>(aiProcessPreset_TargetRealtime_Fast | aiProcess_CalcTangentSpace | aiProcess_ImproveCacheLocality | aiProcess_FindInstances | aiProcess_FixInfacingNormals); 
			bbox bbox_info;
			bool volume_calculated;
			double volume;

	};
}
