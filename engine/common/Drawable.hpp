// This class wraps up verts, indices, render flags, and textures. Transform matrices are handled by the programmer in draw loop
#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <bgfx.h>

#include "Graphics.hpp"
#include "ModelLoader.hpp"
#include "Logger.hpp"

namespace noob
{
	class drawable
	{
		friend class model_loader;

		public:

		uint32_t pack_four_chars_to_uint32(uint8_t _x, uint8_t _y, uint8_t _z, uint8_t _w)
		{
			union
			{
				uint32_t ui32;
				uint8_t arr[4];
			} un;
			un.arr[0] = _x;
			un.arr[1] = _y;
			un.arr[2] = _z;
			un.arr[3] = _w;
			return un.ui32;
		}

		uint32_t pack_floats_to_uint32(float _x, float _y = 0.0f, float _z = 0.0f, float _w = 0.0f)
		{
			const uint8_t xx = uint8_t(_x*127.0f + 128.0f);
			const uint8_t yy = uint8_t(_y*127.0f + 128.0f);
			const uint8_t zz = uint8_t(_z*127.0f + 128.0f);
			const uint8_t ww = uint8_t(_w*127.0f + 128.0f);
			return pack_four_chars_to_uint32(xx, yy, zz, ww);
		}

		struct position_normal_vertex
		{
			float m_x;
			float m_y;
			float m_z;
			uint32_t m_normal;

			static void init()
			{
				ms_decl
					.begin()
					.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
					//.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
					.end();
			}

			static bgfx::VertexDecl ms_decl;
		};
/*
		struct position_normal_uv_vertex
		{
			float m_x;
			float m_y;
			float m_z;
			uint32_t m_normal;

			static void init()
			{
				ms_decl
					.begin()
					.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
					.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
					.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Int16, true, true)
					.end();
			}

			static bgfx::VertexDecl ms_decl;
		};
*/
		drawable();
		drawable(aiScene* scene, aiMesh* drawable, const std::string& filepath);
		~drawable();

		void draw(uint8_t, const float*);
		
		// Candidate for refactoring
		// void load_textures(aiMaterial* mat, aiTextureType type, const std::string& filepath);
		std::vector<bgfx::TextureHandle> textures;

		unsigned int element_count;
		position_normal_vertex* vertices;
		uint16_t* indices;

		bgfx::VertexBufferHandle vertex_buffer;		
		bgfx::IndexBufferHandle index_buffer;
		bgfx::UniformHandle sampler;
		bgfx::TextureHandle texture;
		bgfx::ProgramHandle program_handle;
		uint32_t flags;

		// = bgfx::createVertexBuffer(bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices) ), PosNormalTangentTexcoordVertex::ms_decl);
		// = bgfx::createIndexBuffer(bgfx::makeRef(s_cubeIndices, sizeof(s_cubeIndices) ) );
		// = bgfx::createUniform("u_gradientMap, bgfx::UniformType::Uniform1iv);
	};
}
