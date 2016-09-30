#pragma once

#include <tuple>
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <bgfx/bgfx.h>

#include "NoobDefines.hpp"
#include "NoobUtils.hpp"

namespace noob
{
	class graphics
	{

		static graphics* ptr_to_instance;

		graphics() noexcept(true) : instancing(false) {}

		graphics(const graphics& rhs) noexcept(true)
		{
			ptr_to_instance = rhs.ptr_to_instance;
		}

		graphics& operator=(const graphics& rhs) noexcept(true)
		{
			if (this != &rhs)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}
			return *this;
		}

		~graphics() noexcept(true) {}



		public:

		static graphics& get_instance() noexcept(true)
		{
			static graphics the_instance;
			ptr_to_instance = &the_instance;

			return *ptr_to_instance;
		}


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


		void init(uint32_t width, uint32_t height);
		void destroy();
		void frame(uint32_t width, uint32_t height);

		// ---------------- Asset loaders (builds from files and returns handles) -----------------
		bgfx::ShaderHandle load_shader(const std::string& filename);
		bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);

		// TODO: Implement this 
		// static bgfx::ProgramHandle compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename);

		noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags);

		// ---------------- Asset creators (make assets available from getters) ----------------
		bool add_sampler(const std::string&);
		noob::graphics::uniform add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count);
		bool add_shader(const std::string&, const noob::graphics::shader&);

		// ---------------- Getters -----------------
	//	noob::graphics::shader get_shader(const std::string&) const;
		noob::graphics::texture get_texture(const std::string&) const;
		noob::graphics::uniform get_uniform(const std::string&) const;
		noob::graphics::sampler get_sampler(const std::string&) const;

		// ---------------- Checkers ----------------

		bool is_valid(const noob::graphics::uniform& _uniform) const noexcept(true) { return (_uniform.handle.idx != bgfx::invalidHandle); }
		bool is_valid(const noob::graphics::sampler& _sampler) const noexcept(true) { return (_sampler.handle.idx != bgfx::invalidHandle); }



		// TODO: Implement these
		// static bool is_valid(const noob::graphics::shader&);
		// static bool is_valid(bgfx::ProgramHandle);
		// static bool is_valid(bgfx::ShaderHandle);
		// static bool is_valid(bgfx::TextureHandle);

		// ---------------- Conveniences ------------
		const bgfx::Memory* get_bgfx_mem(const std::string& payload) noexcept(true) { return bgfx::copy(&payload[0], payload.size()); }

		// Uniform getters
		noob::graphics::uniform get_invalid_uniform() const noexcept(true) { return invalid_uniform; }
		noob::graphics::uniform get_colour_0() const noexcept(true) { return colour_0; } 
		noob::graphics::uniform get_colour_1() const noexcept(true) { return colour_1; }
		noob::graphics::uniform get_colour_2() const noexcept(true) { return colour_2; }
		noob::graphics::uniform get_colour_3() const noexcept(true) { return colour_3; }
		noob::graphics::uniform get_blend_0() const noexcept(true) { return blend_0; }
		noob::graphics::uniform get_blend_1() const noexcept(true) { return blend_1; }
		noob::graphics::uniform get_model_scales() const noexcept(true) { return model_scales; }
		noob::graphics::uniform get_tex_scales() const noexcept(true) { return tex_scales; }
		noob::graphics::uniform get_normal_mat() const noexcept(true) { return normal_mat; }
		noob::graphics::uniform get_normal_mat_modelspace() const noexcept(true) { return normal_mat_modelspace; }
		noob::graphics::uniform get_eye_pos() const noexcept(true) { return eye_pos; }
		noob::graphics::uniform get_eye_pos_normalized() const noexcept(true) { return eye_pos_normalized; }
		noob::graphics::uniform get_ambient() const noexcept(true) { return  ambient; }
		noob::graphics::uniform get_light_pos_radius() const noexcept(true) { return light_pos_radius; }
		noob::graphics::uniform get_light_rgb_falloff() const noexcept(true) { return light_rgb_falloff; } 
		noob::graphics::uniform get_specular_shine() const noexcept(true) { return specular_shine; }
		noob::graphics::uniform get_diffuse() const noexcept(true) { return diffuse; }
		noob::graphics::uniform get_emissive() const noexcept(true) { return emissive; }
		noob::graphics::uniform get_fog() const noexcept(true) { return fog; }
		noob::graphics::uniform get_rough_albedo_fresnel() const noexcept(true)  { return rough_albedo_fresnel; }

		noob::graphics::sampler get_invalid_texture() const noexcept(true) { return invalid_texture; }
		noob::graphics::sampler get_texture_0() const noexcept(true) { return texture_0; }

		bool instancing_supported() const noexcept(true) { return instancing; }

		protected:

		bool instancing;
		// Uniforms
		noob::graphics::uniform invalid_uniform, colour_0, colour_1, colour_2, colour_3, blend_0, blend_1, model_scales, tex_scales, normal_mat, normal_mat_modelspace, eye_pos, eye_pos_normalized, ambient, light_pos_radius, light_rgb_falloff, specular_shine, diffuse, emissive, fog, rough_albedo_fresnel;
		noob::graphics::sampler invalid_texture, texture_0;

		// ---------------- Mappings --------------------
		std::unordered_map<std::string, noob::graphics::texture> global_textures;
		std::unordered_map<std::string, noob::graphics::shader> shaders;
		std::unordered_map<std::string, noob::graphics::uniform> uniforms;
		std::unordered_map<std::string, noob::graphics::sampler> samplers;

	};
}
