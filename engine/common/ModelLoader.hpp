/*
#pragma once
#include <unordered_map>
#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
namespace noob
{
	class bone
	{
		public:
			aiMatrix4x4 transformation; 

			unsigned int parent_bone_id; 
			bool has_parent; 

			bone(): has_parent(false) {}
	}; 

	class bone_weights
	{
		public:
			aiMatrix4x4 offsetMatrix; 
			std::vector<aiVertexWeight> weights; 

			unsigned int bone_id; 
	}; 
	class material
	{
		public:
			material() {}
			material(const aimaterial* material) {}
			void bindTexture(aiTextureType textureType, unsigned int textureId) const;
			bool texture(); const {return false;}
	};

	class mesh
	{
		public:
			std::vector<aiVector3D> vertices;
			std::vector<aiVector3D> normals;
			std::vector<aiVector2D> textureCoords;

			std::vector<aiFace> faces;
			std::vector<bone_weights> bone_weights; 
			//In AssImp: one material per mesh
			unsigned int materialId;
	};


	class animation 
	{
		public:
			class Channel
			{
				public:
					unsigned int bone_id; 

					std::vector<aiVectorKey> positions; 
					std::vector<aiQuatKey> rotations; 
					std::vector<aiVectorKey> scales; 
			}; 

			double duration; 
			double ticksPerSecond; 
	};

	class model_loader
	{
		public:

			std::vector<Channel> channels; 
			void load(const std::string& filename);

		protected:
			read(const aiScene* scene);
			unsigned int get_bone_id(const aiNode* node);
			std::vector<animation> animations; 
			std::vector<bone> bones; 
			std::unordered_map<std::string, unsigned int> bone_names_to_id; 

	}
}


*/
