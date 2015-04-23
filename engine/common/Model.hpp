#pragma once

#include <map>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Graphics.hpp"

#include "Logger.hpp"


namespace noob
{
	class model
	{
		public:
			model(const std::string& filename, const std::string& filepath);
			~model(void);
			void render();

			class mesh
			{
				friend class model;
				public:
				~mesh();
				void render();

				private:
				mesh(aiScene* scene, aiMesh* mesh, const std::string& filepath);
				void load_textures(aiMaterial* mat, aiTextureType type, const std::string& filepath);
				std::vector<noob::graphics::texture> textures;
				unsigned int elementCount;
			};

		private:
			std::unique_ptr<std::string> path;
			std::vector<noob::model::mesh*> mesh_entries;
	};
}
