//-----------------------------------------------------------------------------
// Copyright (c) 2015 Andrew Mac
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//-----------------------------------------------------------------------------

#include <bgfx.h>

#include "shaderc.h"
#include "Shader.hpp"

/*
bgfx::UniformHandle shader::textureUniforms[16];
std::map<const std::string&, bgfx::UniformHandle> shader::uniformMap;
// std::array<Shader, 256> shader_list;

uint32_t shaderCount = 0;
std::unique_ptr<std::string> shader_path;
std::unique_ptr<std::string> shaderIncludePath;
std::unique_ptr<std::string> shaderVaryingPath;

void initUniforms()
{
	for(int n = 0; n < 16; ++n)
		shader::textureUniforms[n].idx = bgfx::invalidHandle;
}

void destroyUniforms()
{
	for(int n = 0; n < 16; ++n)
	{
		if ( shader::textureUniforms[n].idx != bgfx::invalidHandle )
			bgfx::destroyUniform(shader::textureUniforms[n]);
	}

	// typedef std::map<const char *, bgfx::UniformHandle, HashTable< const char *, bgfx::UniformHandle> >::iterator it_type;
	for(auto iterator = shader::uniformMap.begin(); iterator != shader::uniformMap.end(); iterator++) 
	{
		if ( iterator->value.idx != bgfx::invalidHandle )
			bgfx::destroyUniform(iterator->value);
	}

	for (uint32_t n = 0; n < shaderCount; ++n)
	{
		shader_list[n].unload();
	}
}

bgfx::UniformHandle shader::get_texture_uniform(uint32_t slot)
{
	if ( slot >= 16 )
	{
		bgfx::UniformHandle dummy;
		dummy.idx = bgfx::invalidHandle;
		return dummy;
	}

	if ( textureUniforms[slot].idx == bgfx::invalidHandle )
	{
		char uniformName[32];
		dSprintf(uniformName, 32, "Texture%d", slot);
		textureUniforms[slot] = bgfx::createUniform(uniformName, bgfx::UniformType::Uniform1i);
	}
	return textureUniforms[slot];
}

bgfx::UniformHandle shader::get_uniform(const std::string& name, bgfx::UniformType::Enum type, uint32_t count)
{
	if ( uniformMap.find(name) == uniformMap.end() ) 
	{
		bgfx::UniformHandle newHandle = bgfx::createUniform(name, type, count);
		uniformMap.insert(std::make_pair(name, newHandle));
	}

	return uniformMap[name];
}
*/
/*
Shader* getShader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
	// Create full shader paths
	std::string full_vs_path;
	dSprintf(full_vs_path, 512, "%s%s", shader_path, vertex_shader_path);
	std::string full_fs_path;
	dSprintf(full_fs_path, 512, "%s%s", shader_path, fragment_shader_path);

	for ( uint32_t n = 0; n < shaderCount; ++n )
	{
		Shader* s = &shader_list[n];
		if ( dStrcmp(s->vertex_shader_path, full_vs_path) == 0 && dStrcmp(s->pixel_shader_path, full_fs_path) == 0 )
			return s;
	}

	if ( shader_list[shaderCount].load(full_vs_path, full_fs_path) )
	{
		shaderCount++;
		return &shader_list[shaderCount - 1];
	}

	return NULL;
}

shader::Shader()
{
	vertex_shader_file = NULL;
	pixel_shader_file = NULL;

	vertex_shader_path = StringTable->EmptyString;
	pixel_shader_path = StringTable->EmptyString;

	pixel_shader.idx = bgfx::invalidHandle;
	vertex_shader.idx = bgfx::invalidHandle;
	shader_program.idx = bgfx::invalidHandle;
}

shader::~Shader()
{
	unload();
}
*/
/*
void shader::unload()
{
	if ( bgfx::isValid(vertex_shader) )
		bgfx::destroyShader(vertex_shader);

	if ( bgfx::isValid(pixel_shader) )
		bgfx::destroyShader(pixel_shader);

	if ( bgfx::isValid(shader_program) )
		bgfx::destroyProgram(shader_program);

	pixel_shader.idx = bgfx::invalidHandle;
	vertex_shader.idx = bgfx::invalidHandle;
	shader_program.idx = bgfx::invalidHandle;
}
*/
//------------------------------------------------------------------------------

/* ShaderAsset::ShaderAsset()
   {
   } */

//------------------------------------------------------------------------------

/* ShaderAsset::~ShaderAsset()
   {
//
} */

//------------------------------------------------------------------------------
/*
void ShaderAsset::initializeAsset( void )
{
	compileAndLoad();
}

void ShaderAsset::setPixelShaderPath( const std::string& _shader_path )
{
	pixel_shader_path = std::unique_ptr<std::string>(new std::string(_shader_path));
}

void ShaderAsset::setVertexShaderPath( const std::string& _shader_path )
{
	pixel_shader_path = std::unique_ptr<std::string>(new std::string(_shader_path));
}

void ShaderAsset::compileAndLoad()
{
	if ( vertex_shader_path->empty() || pixel_shader_path->empty()) 
		return;

	mShader.load(vertex_shader_path, pixel_shader_path);
}
*/
