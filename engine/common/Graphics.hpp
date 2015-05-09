#pragma once

#include <tuple>
#include <map>
#include <bgfx.h>

#include "Shaders.hpp"
#include "NoobUtils.hpp"

namespace noob
{
	class graphics
	{
		public:
			struct pos_norm_uv_bones_vertex
			{
				float m_x;
				float m_y;
				float m_z;
				//float m_tangent_x;
				//float m_tangent_y;
				//float m_tangent_z;
				//float m_bitangent_x;
				//float m_bitangent_y;
				//float m_bitangent_z;
				float m_normal_x;
				float m_normal_y;
				float m_normal_z;
				float m_u;
				float m_v;
				//uint8_t m_boneindex[4];
				//float m_boneweight[4];

				static void init()
				{
					ms_decl
						.begin()
						.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Tangent,   3, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
						//.add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
						.end();
				}
				static bgfx::VertexDecl ms_decl;
			};

			static std::map<std::string, bgfx::TextureHandle> global_textures;
			static std::map<std::string, bgfx::ProgramHandle> programs;
			static std::map<std::string, std::tuple<bgfx::UniformHandle, bgfx::UniformType>> uniforms;
			
			static void init(uint32_t width, uint32_t height);

			static const bgfx::Memory* get_bgfx_mem(const std::string& payload)
			{
				return bgfx::copy(&payload[0], payload.size());
			}

			static void frame(uint32_t width, uint32_t height);

			static bgfx::ShaderHandle load_shader(const std::string& filename);
			static bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);
			static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_file, const std::string& fs_source_file, const std::string& varyings_file);
			static bgfx::TextureHandle load_texture(const std::string& filename, const std::string& name);
			static bgfx::TextureHandle get_texture(const std::string& name);
	};
}
