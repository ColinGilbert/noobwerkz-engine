/*
#include "Graphics.hpp"
#include "Shaders.hpp"

			bgfx::UniformHandle shader::textureUniforms[16];
			std::map<const char*, bgfx::UniformHandle> shader::uniformMap;
			shader shaderList[256];
			uint32_t shaderCount = 0;

			char shaderPath[1024];
			char shaderIncludePath[1024];
			char shaderVaryingPath[1024];

void noob::initUniforms()
{
	for(int n = 0; n < 16; ++n)
		shader::textureUniforms[n].idx = bgfx::invalidHandle;
}
void noob::destroyUniforms()
{
	for(int n = 0; n < 16; ++n)
	{
		if ( shader::textureUniforms[n].idx != bgfx::invalidHandle )
			bgfx::destroyUniform(shader::textureUniforms[n]);
	}
	//typedef std::map<const char *, bgfx::UniformHandle, HashTable< const char *, bgfx::UniformHandle> >::iterator auto;
	for(auto iterator = shader::uniformMap.begin(); iterator != shader::uniformMap.end(); iterator++)
	{
		if ( iterator->value.idx != bgfx::invalidHandle )
			bgfx::destroyUniform(iterator->value);
	}
	for (uint32_t n = 0; n < shaderCount; ++n)
	{
		shaderList[n].unload();
	}
}
bgfx::UniformHandle noob::shader::getTextureUniform(uint32_t slot)
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
bgfx::UniformHandle noob::shader::getUniform(const char* name, bgfx::UniformType::Enum type, uint32_t count)
{
	if ( uniformMap.find(name) == uniformMap.end() )
	{
		bgfx::UniformHandle newHandle = bgfx::createUniform(name, type, count);
		uniformMap.insert(std::make_pair(name, newHandle));
	}
	return uniformMap[name];
}
bgfx::UniformHandle noob::shader::getShadowmapUniform()
{
	return getUniform("ShadowMap", bgfx::UniformType::Uniform1i, 1);
}
bgfx::UniformHandle noob::shader::getUniformVec2(const char* name, uint32_t count)
{
	return getUniform(name, bgfx::UniformType::Uniform2fv, count);
}
bgfx::UniformHandle noob::shader::getUniformVec3(const char* name, uint32_t count)
{
	return getUniform(name, bgfx::UniformType::Uniform3fv, count);
}
bgfx::UniformHandle noob::shader::getUniformVec4(const char* name, uint32_t count)
{
	return getUniform(name, bgfx::UniformType::Uniform4fv, count);
}
bgfx::UniformHandle noob::shader::getUniform4x4Matrix(const char* name, uint32_t count)
{
	return getUniform(name, bgfx::UniformType::Uniform4x4fv, count);
}
noob::shader* noob::getShader(const char* vertex_shader_path, const char* fragment_shader_path)
{
	// Create full shader paths
	char full_vs_path[512];
	dSprintf(full_vs_path, 512, "%s%s", shaderPath, vertex_shader_path);
	char full_fs_path[512];
	dSprintf(full_fs_path, 512, "%s%s", shaderPath, fragment_shader_path);
	for ( uint32_t n = 0; n < shaderCount; ++n )
	{
		shader* s = &shaderList[n];
		if ( dStrcmp(s->mVertexShaderPath, full_vs_path) == 0 && dStrcmp(s->mPixelShaderPath, full_fs_path) == 0 )
			return s;
	}
	if ( shaderList[shaderCount].load(full_vs_path, full_fs_path) )
	{
		shaderCount++;
		return &shaderList[shaderCount - 1];
	}
	return NULL;
}

noob::shader::shader()
{
	mVertexShaderFile = NULL;
	mPixelShaderFile = NULL;
	mVertexShaderPath = StringTable->EmptyString;
	mPixelShaderPath = StringTable->EmptyString;
	mPixelShader.idx = bgfx::invalidHandle;
	mVertexShader.idx = bgfx::invalidHandle;
	mProgram.idx = bgfx::invalidHandle;
}

noob::shader::~shader()
{
	unload();
}

bool noob::shader::load(const char* vertex_shader_path, const char* pixel_shader_path)
{
	unload();
	mVertexShaderPath = StringTable->insert(vertex_shader_path);
	mPixelShaderPath = StringTable->insert(pixel_shader_path);
	bool is_dx9 = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9);
	char shader_output[5000];
	// Vertex shader
	char vertex_compiled_path[256];
	dSprintf(vertex_compiled_path, 256, "%s.bin", vertex_shader_path);
	if ( is_dx9 )
		bgfx::compileShader(0, vertex_shader_path, vertex_compiled_path, "v", "windows", "vs_3_0", NULL, noob::graphics::shader_include_path, noob::graphics::shader_varying_path, shader_output);
	else
		bgfx::compileShader(0, vertex_shader_path, vertex_compiled_path, "v", "linux", NULL, NULL, noob::graphics::shader_include_path, noob::graphics::shader_varying_path, shader_output);
	Con::printf("Compile Vertex shader %s Output: %s", vertex_shader_path, shader_output);
	mVertexShaderFile = new FileObject();
	if ( mVertexShaderFile->readMemory(vertex_compiled_path) )
	{
		const bgfx::Memory* mem = bgfx::makeRef(mVertexShaderFile->getBuffer(), mVertexShaderFile->getBufferSize());
		mVertexShader = bgfx::createShader(mem);
	}
	// Pixel shader
	char pixel_compiled_path[256];
	dSprintf(pixel_compiled_path, 256, "%s.bin", pixel_shader_path);
	if ( is_dx9 )
		bgfx::compileShader(0, pixel_shader_path, pixel_compiled_path, "f", "windows", "ps_3_0", NULL, noob::graphics::shader_include_path, noob::graphics::shader_varying_path, shader_output);
	else
		bgfx::compileShader(0, pixel_shader_path, pixel_compiled_path, "f", "linux", NULL, NULL, noob::graphics::shader_include_path, noob::graphics::shader_varying_path, shader_output);
	Con::printf("Compile Pixel shader %s Output: %s", pixel_shader_path, shader_output);
	mPixelShaderFile = new FileObject();
	if ( mPixelShaderFile->readMemory(pixel_compiled_path) )
	{
		const bgfx::Memory* mem = bgfx::makeRef(mPixelShaderFile->getBuffer(), mPixelShaderFile->getBufferSize());
		mPixelShader = bgfx::createShader(mem);
	}
	// Load Program
	if ( mPixelShader.idx != bgfx::invalidHandle && mVertexShader.idx != bgfx::invalidHandle )
	{
		mProgram = bgfx::createProgram(mVertexShader, mPixelShader, true);
		return bgfx::isValid(mProgram);
	}
	return false;
}

void noob::shader::unload()
{
	}
	if ( bgfx::isValid(mVertexShader) )
		bgfx::destroyShader(mVertexShader);
	if ( bgfx::isValid(mPixelShader) )
		bgfx::destroyShader(mPixelShader);
	if ( bgfx::isValid(mProgram) )
		bgfx::destroyProgram(mProgram);
	mPixelShader.idx = bgfx::invalidHandle;
	mVertexShader.idx = bgfx::invalidHandle;
	mProgram.idx = bgfx::invalidHandle;
}
*/
