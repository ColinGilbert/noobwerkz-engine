#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <bgfx/bgfx.h>

#include <boost/variant/variant.hpp>


#include "NoobUtils.hpp"

namespace noob
{
	class graphics
	{
		public:
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
					handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Int1);
				}

				bgfx::UniformHandle handle;
			};
			
			struct texture
			{
				bgfx::TextureHandle handle;
			};

			struct shader
			{
				bgfx::ProgramHandle program;
				std::vector<noob::graphics::uniform> uniforms;
				std::vector<noob::graphics::sampler> samplers;	
			};

			static void init(uint32_t width, uint32_t height);
			static void destroy();
			static void frame(uint32_t width, uint32_t height);
			
			// ---------------- Asset loaders (builds from files and returns handles) -----------------
			static bgfx::ShaderHandle load_shader(const std::string& filename);
			static bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);

			// TODO: Implement this 
			// static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename);

			static noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags);

			// ---------------- Asset creators (make assets available from getters) ----------------
			static bool add_sampler(const std::string&);
			static noob::graphics::uniform add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count);
			static bool add_shader(const std::string&, const noob::graphics::shader&);

			// ---------------- Getters -----------------
			static noob::graphics::shader get_shader(const std::string&);
			static noob::graphics::texture get_texture(const std::string&);
			static noob::graphics::uniform get_uniform(const std::string&);
			static noob::graphics::sampler get_sampler(const std::string&);

			// ---------------- Checkers ----------------
			static bool is_valid(const noob::graphics::uniform&);
			static bool is_valid(const noob::graphics::sampler&);

			// TODO: Implement these
			// static bool is_valid(const noob::graphics::shader&);
			// static bool is_valid(bgfx::ProgramHandle);
			// static bool is_valid(bgfx::ShaderHandle);
			// static bool is_valid(bgfx::TextureHandle);

			// ---------------- Conveniences ------------
			static const bgfx::Memory* get_bgfx_mem(const std::string& payload)
			{
				return bgfx::copy(&payload[0], payload.size());
			}

			static const noob::graphics::uniform invalid_uniform, colour_0, colour_1, colour_2, colour_3, blend_0, blend_1, tex_scales, normal_mat, normal_mat_modelspace, eye_pos, eye_pos_normalized, global_ambient, light_pos_radius, light_rgb_inner_r, specular_shine, diffuse, ambient, emissive, fog, rough_albedo_fresnel;

			static const noob::graphics::sampler invalid_texture, texture_0;
		protected:
			// ---------------- Data --------------------
			static std::unordered_map<std::string, noob::graphics::texture> global_textures;
			static std::unordered_map<std::string, noob::graphics::shader> shaders;
			static std::unordered_map<std::string, noob::graphics::uniform> uniforms;
			static std::unordered_map<std::string, noob::graphics::sampler> samplers;

	};
}
