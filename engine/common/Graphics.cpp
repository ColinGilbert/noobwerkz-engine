#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Graphics.hpp"

noob::graphics* noob::graphics::ptr_to_instance;

void noob::graphics::init(uint32_t width, uint32_t height)
{
	uint32_t reset = BGFX_RESET_VSYNC;
	bgfx::reset(width, height, reset);

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
	bgfx::setViewClear(1, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x00000000, 1.0f, 0);
	bgfx::setState(BGFX_STATE_DEFAULT);
	
	// Predefined uniforms (declared in `bgfx_shader.sh`):
	//   - `u_viewRect vec4(x, y, width, height)` - view rectangle for current
	//     view.
	//   - `u_viewTexel vec4(1.0/width, 1.0/height, undef, undef)` - inverse
	//     width and height
	//   - `u_view mat4` - view matrix
	//   - `u_invView mat4` - inverted view matrix
	//   - `u_proj mat4` - projection matrix
	//   - `u_invProj mat4` - inverted projection matrix
	//   - `u_viewProj mat4` - concatenated view projection matrix
	//   - `u_invViewProj mat4` - concatenated inverted view projection matrix
	//   - `u_model mat4[BGFX_CONFIG_MAX_BONES]` - array of model matrices.
	//   - `u_modelView mat4` - concatenated model view matrix, only first
	//     model matrix from array is used.
	//   - `u_modelViewProj mat4` - concatenated model view projection matrix.
	//   - `u_alphaRef float` - alpha reference value for alpha test.	
	// Add initial defaults (invalid stuff) to map

	const bgfx::Caps* caps = bgfx::getCaps();

	if (BGFX_CAPS_INSTANCING & caps->supported)
	{
		instancing = true;
	}

	bgfx::ProgramHandle h;
	h.idx = bgfx::invalidHandle;
	noob::graphics::shader shad;
	shad.program = h;

	noob::graphics::add_shader(std::string("invalid"), shad);

	invalid_uniform = noob::graphics::add_uniform(std::string("invalid"), bgfx::UniformType::Enum::Int1, 0);
	
	colour_0 = noob::graphics::add_uniform(std::string("colour_0"), bgfx::UniformType::Enum::Vec4, 1);
	colour_1 = noob::graphics::add_uniform(std::string("colour_1"), bgfx::UniformType::Enum::Vec4, 1);
	colour_2 = noob::graphics::add_uniform(std::string("colour_2"), bgfx::UniformType::Enum::Vec4, 1);
	colour_3 = noob::graphics::add_uniform(std::string("colour_3"), bgfx::UniformType::Enum::Vec4, 1);
	blend_0 = noob::graphics::add_uniform(std::string("blend_0"), bgfx::UniformType::Enum::Vec4, 1);
	blend_1 = noob::graphics::add_uniform(std::string("blend_1"), bgfx::UniformType::Enum::Vec4, 1);
	model_scales = noob::graphics::add_uniform(std::string("model_scales"), bgfx::UniformType::Enum::Vec4, 1);
	tex_scales = noob::graphics::add_uniform(std::string("tex_scales"), bgfx::UniformType::Enum::Vec4, 1);
	normal_mat = add_uniform(std::string("normal_mat"), bgfx::UniformType::Enum::Mat4, 1);
	// transpose(inverse(modelMat) Used to do normal calculations in modelspace.
	normal_mat_modelspace = add_uniform(std::string("u_normal_mat_modelspace"), bgfx::UniformType::Enum::Mat4, 1);
	eye_pos = noob::graphics::add_uniform(std::string("eye_pos"), bgfx::UniformType::Enum::Vec4, 1);
	eye_pos_normalized = noob::graphics::add_uniform(std::string("eye_pos_normalized"), bgfx::UniformType::Enum::Vec4, 1);
	ambient = noob::graphics::add_uniform(std::string("ambient"), bgfx::UniformType::Enum::Vec4, 1);
	light_rgb_falloff = noob::graphics::add_uniform(std::string("u_light_rgb_falloff"), bgfx::UniformType::Enum::Vec4, MAX_LIGHTS);
	light_pos_radius = noob::graphics::add_uniform(std::string("u_light_pos_r"), bgfx::UniformType::Enum::Vec4, MAX_LIGHTS);
	// colour_attenuation = noob::graphics::add_uniform(std::string("colour_attenuation"), bgfx::UniformType::Enum::Vec4, 1);
	// ambient_falloff = noob::graphics::add_uniform(std::string("ambient_falloff"), bgfx::UniformType::Enum::Vec4, 1);
	specular_shine = noob::graphics::add_uniform(std::string("u_specular_shine"), bgfx::UniformType::Enum::Vec4, 1);
	diffuse = noob::graphics::add_uniform(std::string("u_diffuse"), bgfx::UniformType::Enum::Vec4, 1);
	ambient = noob::graphics::add_uniform(std::string("u_ambient"), bgfx::UniformType::Enum::Vec4, 1);
	emissive = noob::graphics::add_uniform(std::string("u_emissive"), bgfx::UniformType::Enum::Vec4, 1);
	fog = noob::graphics::add_uniform(std::string("u_fog"), bgfx::UniformType::Enum::Vec4, 1);
	rough_albedo_fresnel = noob::graphics::add_uniform(std::string("u_rough_albedo_fresnel"), bgfx::UniformType::Enum::Vec4, 1);

	noob::graphics::add_sampler(std::string("invalid"));
	invalid_texture = get_sampler("invalid");
	noob::graphics::add_sampler("texture_0");
	texture_0 = get_sampler("texture_0");
}

void noob::graphics::frame(uint32_t width, uint32_t height)
{
	bgfx::frame();
}


noob::graphics::texture noob::graphics::get_texture(const std::string& name)
{
	if (global_textures.find(name) == global_textures.end())
	{
		bgfx::TextureHandle dummy;
		dummy.idx = bgfx::invalidHandle;
		noob::graphics::texture t;
		t.handle = dummy;
		return t;
	}
	else return global_textures.find(name)->second;
}


bgfx::ShaderHandle noob::graphics::load_shader(const std::string& filename)
{

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

	logger::log(noob::importance::INFO, noob::concat("[Graphics] Loading shader at ", shader_path));

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


noob::graphics::texture noob::graphics::load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags)
{
	int width, height, channels;
	width = height = channels = 0;


	std::string texture_file = noob::utils::load_file_as_string(filename);

	// ww << "Loaded size = " << texture_file.size() << " bytes. ";

	bgfx::TextureInfo tex_info;
	bgfx::TextureHandle tex = bgfx::createTexture(bgfx::copy(&texture_file[0], sizeof(char) * texture_file.size()), flags, 0, &tex_info);

	// bgfx::TextureHandle tex = bgfx::createTexture2D(static_cast<uint16_t>(width), static_cast<uint16_t>(height), static_cast<uint8_t>(0), bgfx::TextureFormat::RGBA32, static_cast<uint32_t>(flags), bgfx::copy(&texture_file[0], sizeof(char) * texture_file.size()));

	// ww << "BGFX texture info: Storage size is " << tex_info.storageSize << " bytes, width of " << tex_info.width << ", height of " << tex_info.height << ", depth of " << tex_info.depth << ". Num mips is " << tex_info.numMips << ". Bpp " << tex_info.bitsPerPixel << ". Cube map? " << tex_info.cubeMap << ".";

	
	logger::log(noob::importance::INFO, noob::concat("[Graphics] Loading Texture - ", filename, ". Loaded size = ", noob::to_string(texture_file.size()), " bytes. BGFX info: ", noob::to_string(tex_info.storageSize), " bytes, width of ", noob::to_string(tex_info.width), ", height of ", noob::to_string(tex_info.height), "depth of ", noob::to_string(tex_info.depth), ". Num mips ", noob::to_string(tex_info.numMips), ". Bpp ", noob::to_string(tex_info.bitsPerPixel), ", cube map? ", noob::to_string(tex_info.cubeMap)));

	noob::graphics::texture t;
	t.handle = tex;
	noob::graphics::global_textures.insert(std::make_pair(friendly_name, t));

	return t;
}


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


noob::graphics::uniform noob::graphics::add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count)
{
	// If item doesn't exists
	if (noob::graphics::uniforms.find(name) == noob::graphics::uniforms.end())
	{
		noob::graphics::uniform uni;
		uni.init(name, type, count);
		noob::graphics::uniforms.insert(std::make_pair(name, uni));
		//return noob::graphics::uniforms[name];
	}
	return noob::graphics::uniforms[name];
	
	//else 
	//{
	//	noob::graphics::uniform u;
	//	return u;
	// }
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
