#include <algorithm>
#include <bgfx.h>

#include "Graphics.hpp"
#include "shaderc.h"

//#define STB_IMAGE_IMPLEMENTATION
#include "stb_impl.h"

bgfx::VertexDecl noob::graphics::pos_norm_uv_bones_vertex::ms_decl;

std::map<std::string, bgfx::TextureHandle> noob::graphics::global_textures;
std::map<std::string, noob::graphics::uniform> noob::graphics::uniforms;
std::map<std::string, noob::graphics::sampler> noob::graphics::samplers;
std::map<std::string, noob::graphics::shader> noob::graphics::shaders;

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

	bgfx::ProgramHandle h;
	h.idx = bgfx::invalidHandle;
	noob::graphics::shader shad;
	shad.program = h;

	noob::graphics::add_shader(std::string("invalid"), shad);

	noob::graphics::add_uniform(std::string("invalid"), bgfx::UniformType::Enum::Uniform1i, 0);

	noob::graphics::add_sampler(std::string("invalid"));
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

bgfx::TextureHandle noob::graphics::load_texture(const std::string& friendly_name, const std::string& filename)
{
	int width = 0;
	int height = 0;
	int channels = 0;
	uint8_t* tex_data = stbi_load(filename.c_str(), &width, &height, &channels, 0); 

	{
		std::stringstream ss;
		ss << "Loading Texture: " << filename << ", width: " << width << ", height: " << height << ", channels: " << channels;
		logger::log(ss.str());
	}

	// TODO: Find out why mips break the rendering 
	bgfx::TextureHandle tex = bgfx::createTexture2D(width, height, 0, bgfx::TextureFormat::Enum::RGBA8, BGFX_TEXTURE_NONE, bgfx::makeRef(tex_data, sizeof(uint8_t) * width * height * channels));

	noob::graphics::global_textures.insert(std::make_pair(friendly_name, tex));

	return tex;
}

// bgfx::ProgramHandle noob::graphics::compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename) {}

bool noob::graphics::add_sampler(const std::string& _name)
{
	// If item doesn't exists
	if (noob::graphics::samplers.find(_name) == noob::graphics::samplers.end())
	{
		noob::graphics::sampler samp;
		samp.init(_name);

		noob::graphics::samplers.insert(std::make_pair(_name, samp));
		return true;
	}
	else return false;
}

bool noob::graphics::add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count)
{
	// If item doesn't exists
	if (noob::graphics::uniforms.find(name) == noob::graphics::uniforms.end())
	{
		noob::graphics::uniform uni;
		uni.init(name, type, count);
		noob::graphics::uniforms.insert(std::make_pair(name, uni));
		return true;
	}
	else return false;

}
/*
bool noob::graphics::add_shader(const std::string& _name, const bgfx::ProgramHandle _program)
{
	std::vector<noob::graphics::uniform> empty_uniforms;
	std::vector<noob::graphics::sampler> empty_samplers;
 	return noob::graphics::add_shader(_name, _program, empty_uniforms, empty_samplers);
}

bool noob::graphics::add_shader(const std::string& _name, const bgfx::ProgramHandle _program, const std::vector<noob::graphics::uniform>& _uniforms)
{
	std::vector<noob::graphics::sampler> empty_samplers;
	return noob::graphics::add_shader(_name, _program, _uniforms, empty_samplers);

}

bool noob::graphics::add_shader(const std::string& _name, const bgfx::ProgramHandle _program, const std::vector<noob::graphics::sampler>& _samplers)
{
	std::vector<noob::graphics::uniform> empty_uniforms;
	return noob::graphics::add_shader(_name, _program, empty_uniforms, _samplers);

}
// TODO: Verify validity prior to insertion (and auto-insert into noob::graphics::uniforms, noob::graphics::samplers, and noob::graphics::programs?)
bool noob::graphics::add_shader(const std::string& _name, const bgfx::ProgramHandle _program, const std::vector<noob::graphics::uniform>& _uniforms, const std::vector<noob::graphics::sampler>& _samplers)
{
	if (noob::graphics::shaders.find(_name) == noob::graphics::shaders.end())
	{
		noob::graphics::shader bundle;
		bundle.program = _program;
		
		for (auto it = _uniforms.begin(); it != _uniforms.end(); ++it)
		{
			bundle.uniforms.push_back(*it); 
		}
		
		for (auto it = _samplers.begin(); it != _samplers.end(); ++it)
		{
			bundle.samplers.push_back(*it); 
		}

		noob::graphics::shaders.insert(std::make_pair(_name, bundle));
		return true;
	}
	else return false;
}
*/

bool noob::graphics::add_shader(const std::string& _name, const noob::graphics::shader& _shader)
{
	if (noob::graphics::shaders.find(_name) == noob::graphics::shaders.end())
	{
		noob::graphics::shaders.insert(std::make_pair(_name, _shader));
		return true;
	}
	else return false;

}

noob::graphics::shader noob::graphics::get_shader(const std::string& name)
{
	auto it = noob::graphics::shaders.find(name);
	if (it != shaders.end()) return it->second;
	else return noob::graphics::shaders.find("invalid")->second;
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
