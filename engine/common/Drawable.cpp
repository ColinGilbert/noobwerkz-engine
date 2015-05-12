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

#include <bgfx.h>
#include <bx/fpumath.h>
#include <bx/timer.h>

// Assimp - Asset Import Library
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/types.h>

#include "Graphics.hpp"
#include "Logger.hpp"
#include "Drawable.hpp"

noob::drawable::drawable() : mesh_filename(std::string("")), scene(NULL)
{
	//   mBoundingBox.minExtents.set(0, 0, 0);
	//   mBoundingBox.maxExtents.set(0, 0, 0);
	mIsAnimated = false;
}

//------------------------------------------------------------------------------

noob::drawable::~drawable()
{
	for ( int32_t m = 0; m < mesh_list.size(); ++m )
	{
		if ( mesh_list[m].mVertexBuffer.idx != bgfx::invalidHandle )
			bgfx::destroyVertexBuffer(mesh_list[m].mVertexBuffer);

		if ( mesh_list[m].mIndexBuffer.idx != bgfx::invalidHandle )
			bgfx::destroyIndexBuffer(mesh_list[m].mIndexBuffer);
	}

	// Clean up.
	if ( scene )
		aiReleaseImport(scene);
}

//------------------------------------------------------------------------------

void noob::drawable::set_mesh_file( const std::string& pMeshFile ) //;const char* pMeshFile )
{
	mesh_filename = pMeshFile;
}

void noob::drawable::load_mesh()
{
	noob::graphics::pos_norm_uv_bones_vertex::init();
	import_mesh();
	process_mesh();
}

void noob::drawable::import_mesh()
{
	//uint64_t hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
	//uint64_t startTime = bx::getHPCounter();

	// Use Assimp To Load Mesh
	scene = aiImportFile(mesh_filename.c_str(), aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_Triangulate | aiProcess_FlipWindingOrder | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	if ( !scene ) return;

	//uint64_t endTime = bx::getHPCounter();
	//Con::printf("ASSIMP IMPORT TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (uint32_t)((endTime - startTime) / hpFreq));

	mIsAnimated = scene->HasAnimations();

	{
		std::stringstream ss;
		ss << "Assimp Scene has " << scene->mNumMeshes << " meshes";
		logger::log(ss.str());
	}

	for( uint32_t m = 0; m < scene->mNumMeshes; ++m )
	{
		aiMesh* mMeshData = scene->mMeshes[m];
		submesh newsubmesh;
		mesh_list.push_back(newsubmesh);
		submesh* subMeshData = &mesh_list[mesh_list.size()-1];

		// Defaults
		//    subMeshData->mBoundingBox.minExtents.set(0, 0, 0);
		//    subMeshData->mBoundingBox.maxExtents.set(0, 0, 0);
		subMeshData->mVertexBuffer.idx = bgfx::invalidHandle;
		subMeshData->mIndexBuffer.idx = bgfx::invalidHandle;
		subMeshData->mMaterialIndex = mMeshData->mMaterialIndex;

		{
			std::stringstream ss;
			ss << "Mesh has " << mMeshData->mNumVertices << " vertices";
			logger::log(ss.str());
		}


		if ( mMeshData->HasTextureCoords(0) ) logger::log("Mesh has texcoords");
		if ( mMeshData->HasNormals() ) logger::log("Mesh has normals");
		if ( mMeshData->HasTangentsAndBitangents() ) logger::log("Mesh has tangents and bitangents");
		{
			std::stringstream ss;
			ss << "Mesh has " << mMeshData->mNumBones << " bones";
			logger::log(ss.str());

		}
		// Verts/UVs/Bones
		for ( uint32_t n = 0; n < mMeshData->mNumVertices; ++n)
		{
			noob::graphics::pos_norm_uv_bones_vertex vert;

			// Verts
			aiVector3D pt = mMeshData->mVertices[n];
			vert.m_x = pt.x;
			vert.m_y = pt.y;
			vert.m_z = pt.z;
			/*
			// Bounding Box
			if ( vert.m_x < subMeshData->mBoundingBox.minExtents.x )
			subMeshData->mBoundingBox.minExtents.x = vert.m_x;
			if ( vert.m_x > subMeshData->mBoundingBox.maxExtents.x )
			subMeshData->mBoundingBox.maxExtents.x = vert.m_x;

			if ( vert.m_y < subMeshData->mBoundingBox.minExtents.y )
			subMeshData->mBoundingBox.minExtents.y = vert.m_y;
			if ( vert.m_y > subMeshData->mBoundingBox.maxExtents.y )
			subMeshData->mBoundingBox.maxExtents.y = vert.m_y;

			if ( vert.m_z < subMeshData->mBoundingBox.minExtents.z )
			subMeshData->mBoundingBox.minExtents.z = vert.m_z;
			if ( vert.m_z > subMeshData->mBoundingBox.maxExtents.z )
			subMeshData->mBoundingBox.maxExtents.z = vert.m_z;
			*/

			// UV

			/*if ( mMeshData->HasTextureCoords(0) )
			  {
			  vert.m_u = mMeshData->mTextureCoords[0][n].x;
			  vert.m_v = mMeshData->mTextureCoords[0][n].y;
			  } */

			// Tangents & Bitangents
			/*		if ( mMeshData->HasTangentsAndBitangents() )
					{
					vert.m_tangent_x = mMeshData->mTangents[n].x;
					vert.m_tangent_y = mMeshData->mTangents[n].y; 
					vert.m_tangent_z = mMeshData->mTangents[n].z; 
					vert.m_bitangent_x = mMeshData->mBitangents[n].x;
					vert.m_bitangent_y = mMeshData->mBitangents[n].y; 
					vert.m_bitangent_z = mMeshData->mBitangents[n].z; 
					} 
					else
					{
					vert.m_tangent_x = 0;
					vert.m_tangent_y = 0; 
					vert.m_tangent_z = 0; 
					vert.m_bitangent_x = 0;
					vert.m_bitangent_y = 0; 
					vert.m_bitangent_z = 0; 
					} */

			// Normals
			if ( mMeshData->HasNormals() )
			{
				vert.m_normal_x = mMeshData->mNormals[n].x;
				vert.m_normal_y = mMeshData->mNormals[n].y; 
				vert.m_normal_z = mMeshData->mNormals[n].z; 
			}
			else 
			{
				// TODO: Better default than zero?
				vert.m_normal_x = 0;
				vert.m_normal_y = 0; 
				vert.m_normal_z = 0; 
			}
			/*
			// Default bone index/weight values.
			vert.m_boneindex[0] = 0;
			vert.m_boneindex[1] = 0;
			vert.m_boneindex[2] = 0;
			vert.m_boneindex[3] = 0;
			vert.m_boneweight[0] = 0.0f;
			vert.m_boneweight[1] = 0.0f;
			vert.m_boneweight[2] = 0.0f;
			vert.m_boneweight[3] = 0.0f;
			*/
			subMeshData->mRawVerts.push_back(vert);
		}
		/*
		// Process Bones/Nodes
		for ( uint32_t n = 0; n < mMeshData->mNumBones; ++n )
		{
		aiBone* boneData = mMeshData->mBones[n];

		// Store bone index by name, and store it's offset matrix.
		uint32_t boneIndex = 0;
		if ( bone_map.find(boneData->mName.C_Str()) == bone_map.end() )
		{
		boneIndex = mBoneOffsets.size();
		bone_map.insert(std::make_pair(boneData->mName.C_Str(), boneIndex));
		mBoneOffsets.push_back(noob::mat4(boneData->mOffsetMatrix));
		}
		else
		{
		boneIndex = bone_map[boneData->mName.C_Str()];
		mBoneOffsets[boneIndex] = noob::mat4(boneData->mOffsetMatrix);
		}

		// Store the bone indices and weights in the vert data.
		for ( uint32_t i = 0; i < boneData->mNumWeights; ++i )
		{
		if ( boneData->mWeights[i].mVertexId >= (uint32_t)subMeshData->mRawVerts.size() ) continue;
		noob::graphics::pos_norm_uv_bones_vertex* vert = &subMeshData->mRawVerts[boneData->mWeights[i].mVertexId];
		for ( uint32_t j = 0; j < 4; ++j )
		{
		if ( vert->m_boneindex[j] == 0 && vert->m_boneweight[j] == 0.0f )
		{
		// TODO: This + 1 is there because we know 0 in the transform table
		// is the main transformation. Maybe this should be done in the 
		// vertex shader instead?
		vert->m_boneindex[j] = boneIndex + 1;
		vert->m_boneweight[j] = boneData->mWeights[i].mWeight / (j + 1);

		// Rescale the previous vert weights.
		for ( uint32_t k = 0; k < j; ++k )
		{
		vert->m_boneweight[k] = vert->m_boneweight[k] * (k + 1);
		vert->m_boneweight[k] = vert->m_boneweight[k] / (j + 1);
		}
		break;
		}
		}
		}
		}
		*/
		{
			std::stringstream ss;
			ss << "Mesh has " << mMeshData->mNumFaces << " faces";
			logger::log(ss.str());
		}
		// Faces
		for ( uint32_t n = 0; n < mMeshData->mNumFaces; ++n)
		{
			const struct aiFace* face = &mMeshData->mFaces[n];
			if ( face->mNumIndices == 2 )
			{
				subMeshData->mRawIndices.push_back(face->mIndices[0]);
				subMeshData->mRawIndices.push_back(face->mIndices[1]);
			}
			else if ( face->mNumIndices == 3 )
			{
				subMeshData->mRawIndices.push_back(face->mIndices[0]);
				subMeshData->mRawIndices.push_back(face->mIndices[1]);
				subMeshData->mRawIndices.push_back(face->mIndices[2]);
			}
			else
			{
				logger::log("[ASSIMP] Non-Triangle Face Found.");
			}
		}
	}
}

void noob::drawable::process_mesh()
{
	//uint64_t hpFreq = bx::getHPFrequency() / 1000000.0; // micro-seconds.
	//uint64_t startTime = bx::getHPCounter();


	for ( int32_t n = 0; n < mesh_list.size(); ++n)
	{
		submesh* subMeshData = &mesh_list[n];

		{
			std::stringstream ss;

			auto num_vertz = subMeshData->mRawVerts.size();
			auto vertz_size = sizeof(noob::graphics::pos_norm_uv_bones_vertex);

			auto num_indicez = subMeshData->mRawIndices.size();
			auto indicez_size = sizeof(uint16_t);

			auto vertz_data_size = num_vertz * vertz_size;
			auto indicez_data_size = num_indicez * indicez_size;

			ss << "Submesh " << n << ". Vertices: " << subMeshData->mRawVerts.size() << " sizeof(vertex) = " << sizeof(noob::graphics::pos_norm_uv_bones_vertex) << ", total data size = " << subMeshData->mRawVerts.size() * sizeof(noob::graphics::pos_norm_uv_bones_vertex) << ". Indices: " << subMeshData->mRawIndices.size() << " sizeof(indices) = " << sizeof(uint16_t) << ", total data size = " << subMeshData->mRawIndices.size() * sizeof(uint16_t);

			logger::log(ss.str());
		}

		// Load the verts and indices into bgfx buffers
		subMeshData->mVertexBuffer = bgfx::createVertexBuffer(bgfx::makeRef(&subMeshData->mRawVerts[0], subMeshData->mRawVerts.size() * sizeof(noob::graphics::pos_norm_uv_bones_vertex)), noob::graphics::pos_norm_uv_bones_vertex::ms_decl);
		subMeshData->mIndexBuffer = bgfx::createIndexBuffer(bgfx::makeRef(&subMeshData->mRawIndices[0], subMeshData->mRawIndices.size() * sizeof(uint16_t) ));


		// subMeshData->mVertexBuffer = bgfx::createVertexBuffer(bgfx::copy(&subMeshData->mRawVerts[0], subMeshData->mRawVerts.size() * sizeof(noob::graphics::pos_norm_uv_bones_vertex)), noob::graphics::pos_norm_uv_bones_vertex::ms_decl);

		//	subMeshData->mIndexBuffer = bgfx::createIndexBuffer(bgfx::copy(&subMeshData->mRawIndices[0], subMeshData->mRawIndices.size() * sizeof(uint16_t) ));



		// Bounding Box
		// mBoundingBox.intersect(subMeshData->mBoundingBox);
	}

	//uint64_t endTime = bx::getHPCounter();
	//Con::printf("PROCESS MESH TOOK: %d microseconds. (1 microsecond = 0.001 milliseconds)", (uint32_t)((endTime - startTime) / hpFreq));
}

void noob::drawable::draw(const noob::mat4& model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT, uint64_t view_id = 0 )
{
	uint32_t num_meshes = get_mesh_count();

	bgfx::submit(view_id);

	for (uint32_t i = 0; i < num_meshes; i++)
	{
		// logger::log("About to draw");
		bgfx::setTransform(&model_mat.m[0]);
		bgfx::setProgram(prog);
		bgfx::setVertexBuffer(get_vertex_buffer(i));
		bgfx::setIndexBuffer(get_index_buffer(i));
		bgfx::setState(bgfx_state_flags);
		bgfx::submit(view_id);
	}

}



/*
// Returns the number of transformations loaded into transformsOut.
uint32_t noob::drawable::get_animated_transforms(double TimeInSeconds, float* transformsOut)
{
if ( !scene ) return 0;

noob::mat4 Identity = noob::identity_mat4();

aiMatrix4x4t<float> rootTransform = scene->mRootNode->mTransformation;
rootTransform.Inverse();
noob::mat4 GlobalInverseTransform = rootTransform;

double TicksPerSecond = scene->mAnimations[0]->mTicksPerSecond != 0 ? scene->mAnimations[0]->mTicksPerSecond : 25.0f;
double TimeInTicks = TimeInSeconds * TicksPerSecond;
double AnimationTime = fmod(TimeInTicks, scene->mAnimations[0]->mDuration);

return read_node_hierarchy(AnimationTime, scene->mRootNode, Identity, GlobalInverseTransform, transformsOut);
}

uint32_t noob::drawable::read_node_hierarchy(double AnimationTime, const aiNode* pNode, noob::mat4 ParentTransform, noob::mat4 GlobalInverseTransform, float* transformsOut)
{ 
uint32_t xfrmCount = 0;
const char* nodeName = pNode->mName.data;
const aiAnimation* pAnimation = scene->mAnimations[0];
const aiNodeAnim* pNodeAnim = _findNodeAnim(pAnimation, nodeName);
noob::mat4 NodeTransformation(pNode->mTransformation);

if ( pNodeAnim ) 
{
// Interpolate scaling and generate scaling transformation matrix
aiVector3D Scaling;
_calcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
noob::mat4 ScalingM;
ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);

// Interpolate rotation and generate rotation transformation matrix
aiQuaternion RotationQ;
_calcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim); 
noob::mat4 RotationM = noob::mat4(RotationQ.GetMatrix());

// Interpolate translation and generate translation transformation matrix
aiVector3D Translation;
_calcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
noob::mat4 TranslationM;
TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);

NodeTransformation = TranslationM * RotationM * ScalingM;
}

noob::mat4 GlobalTransformation = ParentTransform * NodeTransformation;

if ( bone_map.find(nodeName) != bone_map.end() ) 
{
uint32_t BoneIndex = bone_map[nodeName];
xfrmCount = BoneIndex + 1;

noob::mat4 boneTransform = GlobalInverseTransform * GlobalTransformation * mBoneOffsets[BoneIndex];

// Assimp matrices are row-major, we need to transpose to column-major.
boneTransform = transpose(boneTransform);
std::copy(&transformsOut[BoneIndex * 16], &boneTransform.m[0], sizeof(float) * 16); 
}

for ( uint32_t i = 0 ; i < pNode->mNumChildren ; i++ ) 
{
uint32_t childXfrmCount = read_node_hierarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, GlobalInverseTransform, transformsOut);
if ( childXfrmCount > xfrmCount )
xfrmCount = childXfrmCount;
}

return xfrmCount;
}

aiNodeAnim* noob::drawable::_findNodeAnim(const aiAnimation* pAnimation, const std::string& nodeName)
{
	for ( uint32_t n = 0; n < pAnimation->mNumChannels; ++n )
	{
		aiNodeAnim* node = pAnimation->mChannels[n];
		if ( dStrcmp(node->mNodeName.C_Str(), nodeName.c_str()) == 0 )
			return node;
	}

	return NULL;
}

void noob::drawable::_calcInterpolatedRotation(aiQuaternion& Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint32_t RotationIndex = _findRotation(AnimationTime, pNodeAnim);
	uint32_t NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	double DeltaTime = (pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	double Factor = (AnimationTime - pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, (float)Factor);
	Out = Out.Normalize();
}

uint32_t noob::drawable::_findRotation(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint32_t i = 0 ; i < pNodeAnim->mNumRotationKeys - 1 ; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	// TODO: Need Error Handling
	return 0;
}

void noob::drawable::_calcInterpolatedScaling(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint32_t ScalingIndex = _findScaling(AnimationTime, pNodeAnim);
	uint32_t NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	double DeltaTime = (pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	double Factor = (AnimationTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}

uint32_t noob::drawable::_findScaling(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint32_t i = 0 ; i < pNodeAnim->mNumScalingKeys - 1 ; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	// TODO: Need Error Handling
	return 0;
}

void noob::drawable::_calcInterpolatedPosition(aiVector3D& Out, double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint32_t PositionIndex = _findPosition(AnimationTime, pNodeAnim);
	uint32_t NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	double DeltaTime = (pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	double Factor = (AnimationTime - pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + (float)Factor * Delta;
}

uint32_t noob::drawable::_findPosition(double AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumPositionKeys > 0);

	for (uint32_t i = 0 ; i < pNodeAnim->mNumPositionKeys - 1 ; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	// TODO: Need Error Handling
	return 0;
}
*/
