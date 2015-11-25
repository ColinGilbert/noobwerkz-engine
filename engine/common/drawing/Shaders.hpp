/*
#pragma once

#include <map>
#include <bgfx.h>
#include "shaderc.h"

namespace noob
{
	class shader
	{
		public:
			shader();
			~shader();
			bool load(const std::string& vertex_shader_path, const std::string& fragment_shader_path);
			void unload();
			std::string vertex_shader_path, fragment_shader_path, vertex_shader_file, fragment_shader_file;
			bgfx::ShaderHandle vertex_shader;
			bgfx::ShaderHandle pixel_shader;
			bgfx::ProgramHandle program;

			static bgfx::UniformHandle texture_uniforms[16];
			static bgfx::UniformHandle get_shadowmap_uniform();
			static bgfx::UniformHandle get_texture_uniform(uint32_t slot);
			static bgfx::UniformHandle get_uniform(const std::string& name, bgfx::UniformType::Enum type, uint32_t count = 1);
			static bgfx::UniformHandle get_uniform_vec2(const std::string& name, uint32_t count = 1);
			static bgfx::UniformHandle get_uniform_vec3(const std::string& name, uint32_t count = 1);
			static bgfx::UniformHandle get_uniform_vec4(const std::string& name, uint32_t count = 1);
			static bgfx::UniformHandle get_uniform_mat4(const std::string& name, uint32_t count = 1);

			static std::unordered_map<std::string, bgfx::UniformHandle> uniform_map;


		protected:
			noob::shader* get_shader(const std::string&, const std::string&);
			void destroy_uniforms();
			void init_uniforms();


	};
}
*/
