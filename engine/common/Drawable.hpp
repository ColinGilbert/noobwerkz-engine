// Drawable - Loads and encapsulates a mesh. Originally part of Torque6

// Torque 6 copyright info:
//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#pragma once

#include <map>
#include <algorithm>
#include <vector>

#include <bgfx.h>
#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/anim.h>

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Logger.hpp"

namespace noob
{
	class drawable 
	{
		public:
			drawable();
			virtual ~drawable();

			// Mesh Handling.
			void set_mesh_file( const std::string& pMeshFile );
			inline std::string get_mesh_file( void ) const { return mMeshFile; };
			uint32_t get_mesh_count() { return mMeshList.size(); }
			//Box3F getBoundingBox() { return mBoundingBox; }

			// Animation Functions
			uint32_t get_animated_transforms(double TimeInSeconds, float* transformsOut);

			// Buffers
			bgfx::VertexBufferHandle get_vertex_buffer(uint32_t idx) { return mMeshList[idx].mVertexBuffer; }
			bgfx::IndexBufferHandle get_index_buffer(uint32_t idx) { return mMeshList[idx].mIndexBuffer; }
			uint32_t get_material_index(uint32_t idx) { return mMeshList[idx].mMaterialIndex; }

			void load_mesh();

			struct submesh
			{
				std::vector<noob::graphics::pos_norm_uv_bones_vertex> mRawVerts;
				std::vector<uint16_t> mRawIndices;
				bgfx::VertexBufferHandle mVertexBuffer;
				bgfx::IndexBufferHandle mIndexBuffer;
				//Box3F mBoundingBox;
				uint32_t mMaterialIndex;
			};

			void draw(const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT, uint64_t view_id = 0);


		protected:

			void import_mesh();
			void process_mesh();

			/*
			   uint32_t read_node_hierarchy(double AnimationTime, const aiNode* pNode, noob::mat4 ParentTransform, noob::mat4 GlobalInverseTransform, float* transformsOut);
			   aiNodeAnim* _findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);
			   void _calcInterpolatedRotation(aiQuaternion& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
			   uint32_t _findRotation(double AnimationTime, const aiNodeAnim* pNodeAnim);
			   void _calcInterpolatedScaling(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
			   uint32_t _findScaling(double AnimationTime, const aiNodeAnim* pNodeAnim);
			   void _calcInterpolatedPosition(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
			   uint32_t findPosition(double AnimationTime, const aiNodeAnim* pNodeAnim);
			   */

			std::map<const char* , uint32_t> mBoneMap;
			std::vector<noob::mat4> mBoneOffsets;
			std::vector<submesh> mMeshList;
			std::string mMeshFile;
			const aiScene* mScene;
			//Box3F mBoundingBox;
			bool mIsAnimated;

	};
}
