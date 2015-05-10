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

			struct shader_bundle
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

			// TODO: Implement 
			// static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename);

			static bgfx::TextureHandle load_texture(const std::string& name, const std::string& filename);
		
			// ---------------- Asset creators (make assets available from getters) ----------------
			static bool add_sampler(const std::string& name);
			static bool add_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle);
			static bool add_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::uniform>& uniforms);
			static bool add_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::sampler>& samplers);
			static bool add_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::uniform>& uniforms, const std::vector<noob::graphics::sampler>& samplers);
			
			// ---------------- Getters -----------------
			static bgfx::TextureHandle get_texture(const std::string& name);
			static noob::graphics::uniform get_uniform(const std::string& name);
			static noob::graphics::sampler get_sampler(const std::string& name);
			
			// ---------------- Checkers ----------------
			static bool is_valid(const noob::graphics::uniform& _uniform);
			static bool is_valid(const noob::graphics::sampler& _sampler);
			// TODO: Implement
			// static bool is_valid(const noob::graphics::shader_bundle& _bundle);
			// static bool is_valid(bgfx::ProgramHandle handle);
			// static bool is_valid(bgfx::ShaderHandle handle);
			// static bool is_valid(bgfx::TextureHandle handle);

			// ---------------- Conveniences ------------
			static const bgfx::Memory* get_bgfx_mem(const std::string& payload)
			{
				return bgfx::copy(&payload[0], payload.size());
			}

			// ---------------- Data --------------------
			static std::map<std::string, bgfx::TextureHandle> global_textures;
			static std::map<std::string, bgfx::ProgramHandle> programs;
			static std::map<std::string, noob::graphics::shader_bundle> shader_bundles;
			static std::map<std::string, noob::graphics::uniform> uniforms;
			static std::map<std::string, noob::graphics::sampler> samplers;

	};
}
