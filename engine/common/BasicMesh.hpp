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
#include "format.h"
#include <bgfx.h>

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
			//noob::basic_mesh decimate(size_t num_verts) const;

			// Saves to OFF file. First function returns string.
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

			noob::basic_mesh::bbox_info get_bbox() const { return bbox; }

			TriMesh to_half_edges() const;
			void from_half_edges(TriMesh);
			void from_half_edges(PolyMesh);

		protected:
			bbox_info bbox;
			bool volume_calculated;
			double volume;

	};
}
