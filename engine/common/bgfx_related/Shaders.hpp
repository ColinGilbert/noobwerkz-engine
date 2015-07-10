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
			bool load(const char* vertex_shader_path, const char* fragment_shader_path);
			void unload();
			StringTableEntry mVertexShaderPath;
			StringTableEntry mPixelShaderPath;
			FileObject* mVertexShaderFile;
			FileObject* mPixelShaderFile;
			bgfx::ShaderHandle mVertexShader;
			bgfx::ShaderHandle mPixelShader;
			bgfx::ProgramHandle mProgram;

			static bgfx::UniformHandle textureUniforms[16];
			static bgfx::UniformHandle getShadowmapUniform();
			static bgfx::UniformHandle getTextureUniform(uint32_t slot);
			static bgfx::UniformHandle getUniform(const char* name, bgfx::UniformType::Enum type, uint32_t count = 1);
			static bgfx::UniformHandle getUniformVec2(const char* name, uint32_t count = 1);
			static bgfx::UniformHandle getUniformVec3(const char* name, uint32_t count = 1);
			static bgfx::UniformHandle getUniformVec4(const char* name, uint32_t count = 1);
			static bgfx::UniformHandle getUniform4x4Matrix(const char* name, uint32_t count = 1);

			static std::map<const char*, bgfx::UniformHandle> uniformMap;


		protected:
			noob::shader* getShader(const char*, const char*);
			void destroyUniforms();
			void initUniforms();


	};
}
*/
