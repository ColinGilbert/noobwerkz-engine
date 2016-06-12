// TODO: Replace hard-coded cylinder, cone, sphere functions with more generic swept shape algorithm. Low priority.

#pragma once

#define OM_STATIC_BUILD 1

#include <OpenMesh/Core/IO/MeshIO.hh>
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

typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;
typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;

#include <assimp/quaternion.h>
#include <assimp/anim.h>
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

#include "MathFuncs.hpp"
#include "TransformHelper.hpp"

#include "Logger.hpp"

namespace noob
{
	class basic_mesh 
	{
		friend class stage;
		friend class basic_model;
		friend class active_mesh;
		friend class shape;
		friend class mesh_utils;
		friend class voxel_world;
		friend class voxel_world_stable;
		
		public:
			
			basic_mesh() : volume_calculated(false), volume(0.0) {}
			
			noob::vec3 get_vertex(unsigned int);
			// noob::vec3 get_normal(unsigned int);
			// noob::vec3 get_texcoord(unsigned int);
			unsigned int get_index(unsigned int);
			
			void set_vertex(unsigned int, const noob::vec3&);
			// void set_normal(unsigned int, const noob::vec3&);
			// void set_texcoord(unsigned int, const noob::vec3&);
			void set_index(unsigned int, unsigned int);
			
			double get_volume();
			
			noob::basic_mesh decimate(size_t num_verts) const;
			
			std::string save() const;
			void save(const std::string& filename) const;
			
			bool load_mem(const std::string&, const std::string& name = "");
			bool load_file(const std::string& filename, const std::string& name = "");
			
			// void normalize();
			// void transform(const noob::mat4& transform);
			// void to_origin();
			// void translate(const noob::vec3&);
			// void rotate(const noob::versor&);
			// void scale(const noob::vec3&);
			
			noob::bbox get_bbox() const { return bbox; }
			
			
		protected:
			bool load_assimp(const aiScene* scene, const std::string& name);
		
			TriMesh to_half_edges() const;
			
			void from_half_edges(TriMesh);
			void from_half_edges(PolyMesh);
			
			// void vert_normals_from_trimesh(TriMesh);
			
			std::vector<noob::vec3> vertices;
			// std::vector<noob::vec3> normals;
			// std::vector<noob::vec3> texcoords;
			std::vector<uint32_t> indices;
			
			noob::bbox bbox;
			bool volume_calculated;
			double volume;
	};
}
