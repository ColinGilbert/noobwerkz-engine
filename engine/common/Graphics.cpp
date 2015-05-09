#include <algorithm>

#include "Graphics.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bgfx::VertexDecl noob::graphics::pos_norm_uv_bones_vertex::ms_decl;

std::map<std::string, bgfx::TextureHandle> noob::graphics::global_textures;
std::map<std::string, bgfx::ProgramHandle> noob::graphics::programs;
std::map<std::string, std::tuple<bgfx::UniformHandle, bgfx::UniformType>> noob::graphics::uniforms;

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

bgfx::TextureHandle noob::graphics::load_texture(const std::string& filename, const std::string& name)
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

bgfx::ProgramHandle noob::graphics::compile_and_load_program(const std::string& vs_source_filename, const std::string& fs_source_filename, const std::string& varyings_filename)
{

}
