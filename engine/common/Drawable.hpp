// Drawable - Loads and encapsulates a mesh. Lots of inspiration taken from Torque6

#pragma once

#include <map>
#include <algorithm>
#include <vector>
#include <bgfx.h>

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

// TODO: Separate this class into Drawable and mesh loader

#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/anim.h>

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Logger.hpp"
#include "Mesh.hpp"

namespace noob
{
	class drawable 
	{
		public:
			drawable();
			drawable(noob::mesh mesh);
			drawable(std::vector<noob::mesh> mesh_vector);
			virtual ~drawable();

			// Mesh Handling.
			void set_mesh_file( const std::string& pMeshFile );
			inline std::string get_mesh_file( void ) const { return mesh_filename; };
			uint32_t get_mesh_count() { return mesh_list.size(); }
			//Box3F getBoundingBox() { return mBoundingBox; }

			// Animation Functions
			uint32_t get_animated_transforms(double TimeInSeconds, float* transformsOut);

			// Buffers
			bgfx::VertexBufferHandle get_vertex_buffer(uint32_t idx) { return mesh_list[idx].vertex_buffer; }
			bgfx::IndexBufferHandle get_index_buffer(uint32_t idx) { return mesh_list[idx].index_buffer; }

			uint32_t get_material_index(uint32_t idx) { return mesh_list[idx].material_index; }

			void load_mesh();

			void draw(const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT, uint64_t view_id = 0);
			std::vector<noob::mesh> mesh_list;
	
		protected:
			void import_mesh();
			void process_mesh();

			uint32_t _findRotation(double AnimationTime, const aiNodeAnim* pNodeAnim);
			void _calcInterpolatedScaling(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
			uint32_t _findScaling(double AnimationTime, const aiNodeAnim* pNodeAnim);
			void _calcInterpolatedPosition(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
			uint32_t findPosition(double AnimationTime, const aiNodeAnim* pNodeAnim); 
			std::map<const char* , uint32_t> bone_map;
			std::vector<noob::mat4> bone_offsets;
			
			std::string mesh_filename;
			const aiScene* scene;
			//Box3F mBoundingBox;
			bool mIsAnimated;
	};
}
