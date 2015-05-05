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

#include <bgfx.h>
#include <assimp/scene.h>
#include <assimp/quaternion.h>
#include <assimp/anim.h>

#include <map>
#include <utility>
#include <algorithm>
#include <vector>

#include "Graphics.hpp"
#include "MathFuncs.hpp"

namespace noob
{
				class drawable 
				{
								public:
												drawable();
												virtual ~drawable();

												// Mesh Handling.
												void setMeshFile( const std::string& pMeshFile );
												inline std::string getMeshFile( void ) const { return mMeshFile; };
												uint32_t getMeshCount() { return mMeshList.size(); }
												//Box3F getBoundingBox() { return mBoundingBox; }

												// Animation Functions
												uint32_t getAnimatedTransforms(double TimeInSeconds, float* transformsOut);

												// Buffers
												bgfx::VertexBufferHandle getVertexBuffer(uint32_t idx) { return mMeshList[idx].mVertexBuffer; }
												bgfx::IndexBufferHandle getIndexBuffer(uint32_t idx) { return mMeshList[idx].mIndexBuffer; }
												uint32_t getMaterialIndex(uint32_t idx) { return mMeshList[idx].mMaterialIndex; }

												void loadMesh();

												typedef struct
												{
																std::vector<noob::graphics::PosUVTBNBonesVertex> mRawVerts;
																std::vector<uint16_t> mRawIndices;
																bgfx::VertexBufferHandle mVertexBuffer;
																bgfx::IndexBufferHandle mIndexBuffer;
																//Box3F mBoundingBox;
																uint32_t mMaterialIndex;
												} SubMesh;

								protected:

												// Mesh Handling
												
												void importMesh();
												void processMesh();

								/*				// Animation Functions.
												uint32_t read_node_hierarchy(double AnimationTime, const aiNode* pNode, noob::mat4 ParentTransform, noob::mat4 GlobalInverseTransform, float* transformsOut);
												aiNodeAnim* _findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName);
												void _calcInterpolatedRotation(aiQuaternion& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
												uint32_t _findRotation(double AnimationTime, const aiNodeAnim* pNodeAnim);
												void _calcInterpolatedScaling(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
												uint32_t _findScaling(double AnimationTime, const aiNodeAnim* pNodeAnim);
												void _calcInterpolatedPosition(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim);
												uint32_t findPosition(double AnimationTime, const aiNodeAnim* pNodeAnim);
*/
								private:
												std::map<const char* , uint32_t> mBoneMap;
												std::vector<noob::mat4> mBoneOffsets;
												std::vector<SubMesh> mMeshList;
												std::string mMeshFile;
												const aiScene* mScene;
												//Box3F mBoundingBox;
												bool mIsAnimated;

				};
}
