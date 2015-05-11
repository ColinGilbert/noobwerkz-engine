#pragma once

#include <tuple>
#include <map>
#include <vector>

#include <bgfx.h>
#include "Shaders.hpp"
#include "NoobUtils.hpp"

namespace noob
{
	class graphics
	{
		public:

			// ---------------- Structs ------------------
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

			struct uniform
			{
				void init(const std::string& name, bgfx::UniformType::Enum _type, uint16_t _count = 1)
				{
					handle = bgfx::createUniform(name.c_str(), type, _count);
					type = _type;
					count = _count;
				}

				bgfx::UniformHandle handle;
				bgfx::UniformType::Enum type;
				uint16_t count;
			};

			struct sampler
			{
				void init(const std::string& name)
				{
					handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Uniform1iv);
				}

				bgfx::UniformHandle handle;
			};

			struct shader
			{
				bgfx::ProgramHandle program_handle;
				std::vector<noob::graphics::uniform> uniforms;
				std::vector<noob::graphics::sampler> samplers;	
			};
			
			static void init(uint32_t width, uint32_t height);
			static void frame(uint32_t width, uint32_t height);

			// ---------------- Asset loaders (builds from files and returns handles) -----------------
			static bgfx::ShaderHandle load_shader(const std::string& filename);
			static bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);

			// TODO: Implement this 
			// static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename);

			static bgfx::TextureHandle load_texture(const std::string& name, const std::string& filename);
		
			// ---------------- Asset creators (make assets available from getters) ----------------
			static bool add_sampler(const std::string&);
			static bool add_uniform(const std::string&, bgfx::UniformType::Enum, uint16_t);
			static bool add_shader(const std::string&, const bgfx::ProgramHandle);
			static bool add_shader(const std::string&, const bgfx::ProgramHandle, const std::vector<noob::graphics::uniform>&);
			static bool add_shader(const std::string&, const bgfx::ProgramHandle, const std::vector<noob::graphics::sampler>&);
			static bool add_shader(const std::string&, const bgfx::ProgramHandle, const std::vector<noob::graphics::uniform>&, const std::vector<noob::graphics::sampler>&);
			
			// ---------------- Getters -----------------
			static bgfx::TextureHandle get_texture(const std::string&);
			static noob::graphics::uniform get_uniform(const std::string&);
			static noob::graphics::sampler get_sampler(const std::string&);
			
			// ---------------- Checkers ----------------
			static bool is_valid(const noob::graphics::uniform&);
			static bool is_valid(const noob::graphics::sampler&);

			// TODO: Implement these
			// static bool is_valid(const noob::graphics::shader&);
			// static bool is_valid(const noob::uniform&);
			// static bool is_valid(const noob::sampler&);
			// static bool is_valid(bgfx::ProgramHandle);
			// static bool is_valid(bgfx::ShaderHandle);
			// static bool is_valid(bgfx::TextureHandle);


			// ---------------- Conveniences ------------
			static const bgfx::Memory* get_bgfx_mem(const std::string& payload)
			{
				return bgfx::copy(&payload[0], payload.size());
			}

			// ---------------- Data --------------------
			static std::map<std::string, bgfx::TextureHandle> global_textures;
			static std::map<std::string, bgfx::ProgramHandle> programs;
			static std::map<std::string, noob::graphics::shader> shaders;
			static std::map<std::string, noob::graphics::uniform> uniforms;
			static std::map<std::string, noob::graphics::sampler> samplers;

	};
}
