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

				bgfx::VertexBufferHandle vertex_buffer;


				// = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) ), PosNormalTangentTexcoordVertex::ms_decl);

				// Create static index buffer.
				bgfx::IndexBufferHandle index_buffer;

				// = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) ) );

				// Create texture sampler uniforms.
				bgfx::UniformHandle texture_uniform;

				//= bgfx::createUniform("u_gradientMap, bgfx::UniformType::Uniform1iv);

				bgfx::VertexDecl PosNormalTexcoordVertex::ms_decl;
				protected:
				std::unique_ptr<std::string> path;
				std::vector<noob::model::mesh*> mesh_entries;

				struct PosNormalTexcoordVertex
				{
					float m_x;
					float m_y;
					float m_z;
					uint32_t m_normal;
					//uint32_t m_tangent;
					int16_t m_u;
					int16_t m_v;
					static void init()
					{
						ms_decl
							.begin()
							.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
							.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
							//.add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Uint8, true, true)
							.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
							.end();
					}
					static bgfx::VertexDecl ms_decl;
				};

			};
	}
