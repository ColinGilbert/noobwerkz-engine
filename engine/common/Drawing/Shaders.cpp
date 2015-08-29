/*
#include "Graphics.hpp"
#include "Shaders.hpp"

bgfx::UniformHandle shader::texture_uniforms[16];
std::map<const std::string&, bgfx::UniformHandle> shader::uniformMap;
shader shaderList[256];
uint32_t shaderCount = 0;

char shaderPath[1024];
char shaderIncludePath[1024];
char shaderVaryingPath[1024];

void noob::initUniforms()
{
	for(int n = 0; n < 16; ++n)
		shader::texture_uniforms[n].idx = bgfx::invalidHandle;
}

void noob::destroyUniforms()
{
	for(int n = 0; n < 16; ++n)
	{
		if ( shader::texture_uniforms[n].idx != bgfx::invalidHandle )
			bgfx::destroyUniform(shader::texture_uniforms[n]);
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

bgfx::UniformHandle noob::shader::get_texture_uniform(uint32_t slot)
{
	if ( slot >= 16 )
	{
		bgfx::UniformHandle dummy;
		dummy.idx = bgfx::invalidHandle;
		return dummy;
	}
	if ( texture_uniforms[slot].idx == bgfx::invalidHandle )
	{
		char uniformName[32];
		dSprintf(uniformName, 32, "Texture%d", slot);
		texture_uniforms[slot] = bgfx::createUniform(uniformName, bgfx::UniformType::Uniform1i);
	}
	return texture_uniforms[slot];
}

bgfx::UniformHandle noob::shader::get_uniform(const std::string& name, bgfx::UniformType::Enum type, uint32_t count)
{
	if ( uniformMap.find(name) == uniformMap.end() )
	{
		bgfx::UniformHandle newHandle = bgfx::createUniform(name, type, count);
		uniformMap.insert(std::make_pair(name, newHandle));
	}
	return uniformMap[name];
}

bgfx::UniformHandle noob::shader::get_shadowmap_uniform()
{
	return get_uniform("ShadowMap", bgfx::UniformType::Uniform1i, 1);
}

bgfx::UniformHandle noob::shader::get_uniform_vec2(const std::string& name, uint32_t count)
{
	return get_uniform(name, bgfx::UniformType::Uniform2fv, count);
}

bgfx::UniformHandle noob::shader::get_uniform_vec3(const std::string& name, uint32_t count)
{
	return get_uniform(name, bgfx::UniformType::Uniform3fv, count);
}

bgfx::UniformHandle noob::shader::get_uniform_vec4(const std::string& name, uint32_t count)
{
	return get_uniform(name, bgfx::UniformType::Uniform4fv, count);
}

bgfx::UniformHandle noob::shader::get_uniform_mat4(const std::string& name, uint32_t count)
{
	return get_uniform(name, bgfx::UniformType::Uniform4x4fv, count);
}

noob::shader* noob::get_shader(const std::string& vertex_shader_path, const std::string& fragment_shader_path)
{
	// Create full shader paths
	char full_vs_path[512];
	dSprintf(full_vs_path, 512, "%s%s", shaderPath, vertex_shader_path.c_str());
	char full_fs_path[512];
	dSprintf(full_fs_path, 512, "%s%s", shaderPath, fragment_shader_path.c_str());
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
	mPixelShader.idx = bgfx::invalidHandle;
	mVertexShader.idx = bgfx::invalidHandle;
	mProgram.idx = bgfx::invalidHandle;
}

noob::shader::~shader()
{
	unload();
}

bool noob::shader::load(const std::string& vertex_shader_path, const std::string& pixel_shader_path)
{
	unload();
	bool is_dx9 = (bgfx::getRendererType() == bgfx::RendererType::Direct3D9);
	char shader_output[5000];
	// Vertex shader
	char vertex_compiled_path[256];
	dSprintf(vertex_compiled_path, 256, "%s.bin", vertex_shader_path.c_str());
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
