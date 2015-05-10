#include <algorithm>
#include <bgfx.h>

#include "Graphics.hpp"
#include "shaderc.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bgfx::VertexDecl noob::graphics::pos_norm_uv_bones_vertex::ms_decl;

std::map<std::string, bgfx::TextureHandle> noob::graphics::global_textures;
std::map<std::string, bgfx::ProgramHandle> noob::graphics::programs;
std::map<std::string, noob::graphics::uniform> noob::graphics::uniforms;
std::map<std::string, noob::graphics::sampler> noob::graphics::samplers;
std::map<std::string, noob::graphics::shader_bundle> noob::graphics::shader_bundles;

void noob::graphics::init(uint32_t width, uint32_t height)
{
	uint32_t reset = BGFX_RESET_VSYNC;

	bgfx::reset(width, height, reset);

	bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x703070ff
			, 1.0f
			, 0
			);

	noob::graphics::shader_bundle invalid_shader;
	invalid_shader.program_handle.idx = bgfx::invalidHandle;
	noob::graphics::shader_bundles.insert(std::make_pair(std::string("invalid"), invalid_shader));

	noob::graphics::uniform invalid_uniform;
	invalid_uniform.handle.idx = bgfx::invalidHandle;
	invalid_uniform.count = 0;
	invalid_uniform.type = bgfx::UniformType::Enum::Uniform1i;
	noob::graphics::uniforms.insert(std::make_pair(std::string("invalid"),invalid_uniform));

	noob::graphics::sampler invalid_sampler;
	invalid_sampler.handle.idx = bgfx::invalidHandle;
	noob::graphics::samplers.insert(std::make_pair(std::string("invalid"), invalid_sampler));
}

void noob::graphics::frame(uint32_t width, uint32_t height)
{
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::submit(0);
	bgfx::frame();
}

bgfx::TextureHandle noob::graphics::get_texture(const std::string& name)
{
	if (global_textures.find(name) == global_textures.end())
	{
		bgfx::TextureHandle dummy;
		dummy.idx = bgfx::invalidHandle;
		return dummy;
	}
	else return global_textures.find(name)->second;
}

bgfx::ShaderHandle noob::graphics::load_shader(const std::string& filename)
{
	logger::log("Loading shader");
	std::string shader_path = "shaders/dx9/";

	switch (bgfx::getRendererType() )
	{
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			shader_path = "shaders/dx11/";
			break;

		case bgfx::RendererType::OpenGL:
			shader_path = "shaders/glsl/";
			break;

		case bgfx::RendererType::OpenGLES:
			shader_path = "shaders/gles/";
			break;

		default:
			break;
	}

	shader_path.append(filename);
	shader_path.append(".bin");

	logger::log(shader_path);
	// noob::utils::data.insert(std::make_pair(shader_path, noob::utils::load_file_as_string(shader_path)));
	const bgfx::Memory* mem = get_bgfx_mem(noob::utils::load_file_as_string(shader_path));
	bgfx::ShaderHandle s = bgfx::createShader(mem);

	return s;
}

bgfx::ProgramHandle noob::graphics::load_program(const std::string& vs_filename, const std::string& fs_filename)
{

	bgfx::ShaderHandle vsh = load_shader(vs_filename);
	bgfx::ShaderHandle fsh = load_shader(fs_filename);
	return bgfx::createProgram(vsh, fsh, true);
}

bgfx::TextureHandle noob::graphics::load_texture(const std::string& name, const std::string& filename)
{
	int width = 0;
	int height = 0;
	int channels = 0;
	uint8_t* tex_data = stbi_load(filename.c_str(), &width, &height, &channels, 0); 

	uint16_t smallest_side = std::min(width, height);
	uint8_t mips;

	for (float tiny = smallest_side; tiny > 1.0; tiny /= 2.0)
	{
		mips++;
	}

	bgfx::TextureHandle tex = bgfx::createTexture2D(width, height, mips, bgfx::TextureFormat::Enum::RGBA8, BGFX_TEXTURE_NONE, bgfx::makeRef(tex_data, sizeof(uint8_t) * width * height * channels));

	noob::graphics::global_textures.insert(std::make_pair(name, tex));

	return tex;
}

/*
   bgfx::ProgramHandle noob::graphics::compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename)
   {

   }
   */

bool noob::graphics::create_sampler(const std::string& name)
{
	// If item doesn't exists
	if (samplers.find(name) == samplers.end())
	{
		noob::graphics::sampler samp;
		samp.init(name);

		noob::graphics::samplers.insert(std::make_pair(name, samp));
		return true;
	}
	else return false;
}

bool noob::graphics::create_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle)
{
	std::vector<noob::graphics::uniform> empty_uniforms;
	std::vector<noob::graphics::sampler> empty_samplers;
 	return noob::graphics::create_shader_bundle(name, program_handle, empty_uniforms, empty_samplers);
}

bool noob::graphics::create_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::uniform>& uniforms)
{
	std::vector<noob::graphics::sampler> empty_samplers;
	return noob::graphics::create_shader_bundle(name, program_handle, uniforms, empty_samplers);

}

bool noob::graphics::create_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::sampler>& samplers)
{
	std::vector<noob::graphics::uniform> empty_uniforms;
	return noob::graphics::create_shader_bundle(name, program_handle, empty_uniforms, samplers);

}

// TODO: Verify validity prior to insertion (and auto-insert into noob::graphics::uniforms, noob::graphics::samplers, and noob::graphics::programs?)
bool noob::graphics::create_shader_bundle(const std::string& name, const bgfx::ProgramHandle program_handle, const std::vector<noob::graphics::uniform>& uniforms, const std::vector<noob::graphics::sampler>& samplers)
{
	if (noob::graphics::shader_bundles.find(name) == noob::graphics::shader_bundles.end())
	{
		noob::graphics::shader_bundle bundle;
		bundle.program_handle = program_handle;
		
		for (auto it = uniforms.begin(); it !=uniforms.end(); ++it)
		{
			bundle.uniforms.push_back(*it); 
		}
		
		for (auto it = samplers.begin(); it !=samplers.end(); ++it)
		{
			bundle.samplers.push_back(*it); 
		}

		noob::graphics::shader_bundles.insert(std::make_pair(name, bundle));
		return true;
	}
	else return false;
}

noob::graphics::uniform noob::graphics::get_uniform(const std::string& name)
{
	auto it = noob::graphics::uniforms.find(name);
	if (it != uniforms.end()) return it->second;
	else return noob::graphics::uniforms.find("invalid")->second;
}

noob::graphics::sampler noob::graphics::get_sampler(const std::string& name)
{
	auto it = noob::graphics::samplers.find(name);
	if (it != samplers.end()) return it->second;
	else return noob::graphics::samplers.find("invalid")->second;
}

bool is_valid(const noob::graphics::uniform& _uniform)
{
	return (_uniform.handle.idx != bgfx::invalidHandle);
}

bool is_valid(const noob::graphics::sampler& _sampler)
{
	return (_sampler.handle.idx != bgfx::invalidHandle);
}
