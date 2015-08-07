/*
#include "ModelLoader.hpp"


void noob::model_loader::read(const aiScene* scene)
{
	//Find channels, and the bones used in the channels
	for(unsigned int ca = 0; ca<scene->mNumAnimations; ca++)
	{
		animations.emplace_back(); 
		auto& animation = animations[ca]; 

		animation.duration = scene->mAnimations[ca]->mDuration; 
		animation.ticksPerSecond = scene->mAnimations[ca]->mTicksPerSecond; 

		animation.channels.resize(scene->mAnimations[ca]->mNumChannels); 
		for(unsigned int cc = 0; cc<scene->mAnimations[ca]->mNumChannels; cc++)
		{
			animation.channels[cc].positions.resize(scene->mAnimations[ca]->mChannels[cc]->mNumPositionKeys); 
			animation.channels[cc].scales.resize(scene->mAnimations[ca]->mChannels[cc]->mNumScalingKeys); 
			animation.channels[cc].rotations.resize(scene->mAnimations[ca]->mChannels[cc]->mNumRotationKeys); 

			for(unsigned int cp = 0; cp<scene->mAnimations[ca]->mChannels[cc]->mNumPositionKeys; cp++)
			{
				animation.channels[cc].positions[cp] = scene->mAnimations[ca]->mChannels[cc]->mPositionKeys[cp]; 
			}
			for(unsigned int cs = 0; cs<scene->mAnimations[ca]->mChannels[cc]->mNumScalingKeys; cs++)
			{
				animation.channels[cc].scales[cs] = scene->mAnimations[ca]->mChannels[cc]->mScalingKeys[cs]; 
			}
			for(unsigned int cr = 0; cr<scene->mAnimations[ca]->mChannels[cc]->mNumRotationKeys; cr++) 
			{
				animation.channels[cc].rotations[cr] = scene->mAnimations[ca]->mChannels[cc]->mRotationKeys[cr]; 
			}

			const aiNode* node = scene->mRootNode->FindNode(scene->mAnimations[ca]->mChannels[cc]->mNodeName); 
			animations[ca].channels[cc].bone_id = get_bone_id(node); 
		}
	}

	//Find all the bones, and their parent bones, connected to the meshes
	for(unsigned int cm = 0; cm<scene->mNumMeshes; cm++)
	{
		for(unsigned int cb = 0; cb<scene->mMeshes[cm]->mNumBones; cb++)
		{
			const aiNode* node = scene->mRootNode->FindNode(scene->mMeshes[cm]->mBones[cb]->mName);                     
			this->meshes[cm].bone_weights.emplace_back(); 
			unsigned int bone_id = get_bone_id(node); 
			this->meshes[cm].bone_weights[cb].bone_id = bone_id; 
			this->meshes[cm].bone_weights[cb].offsetMatrix = scene->mMeshes[cm]->mBones[cb]->mOffsetMatrix; 

			for(unsigned int cw = 0; cw<scene->mMeshes[cm]->mBones[cb]->mNumWeights; cw++)
			{
				this->meshes[cm].bone_weights[cb].weights.emplace_back(scene->mMeshes[cm]->mBones[cb]->mWeights[cw]); 
			}

			if(!bones[bone_id].has_parent)
			{
				//Populate Bone::parent_bone_ids
				node = node->mParent; 
				while(node! = scene->mRootNode)
				{
					unsigned int parent_bone_id = get_bone_id(node); 
					bones[bone_id].parent_bone_id = parent_bone_id; 
					bones[bone_id].has_parent = true; 
					bone_id = parent_bone_id; 

					node = node->mParent; 
				}
			}
		}
	}
} 

unsigned int noob::model_loader::get_bone_id(const aiNode* node)
{
	unsigned int bone_id; 
	if(bone_names_to_id.count(node->mName.data) =  = 0)
	{
		bones.emplace_back(); 
		bone_id = bones.size()-1; 
		bone_names_to_id[node->mName.data] = bone_id; 

		bones[bone_id].transformation = node->mTransformation; 
	}
	else
	{
		bone_id = bone_names_to_id[node->mName.data]; 
	}

	return bone_id; 
}
*/
