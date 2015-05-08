#include <algorithm>

#include "Graphics.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bgfx::VertexDecl noob::graphics::pos_norm_uv_bones_vertex::ms_decl;

std::map<std::string, bgfx::TextureHandle> noob::graphics::global_textures;
std::map<std::string, bgfx::ProgramHandle> noob::graphics::programs;

std::unique_ptr<std::string> noob::graphics::shader_include_path;
std::unique_ptr<std::string> noob::graphics::shader_varying_path;

bgfx::TextureHandle noob::graphics::load_texture(const std::string& filename, const std::string& friendly_name)
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

	noob::graphics::global_textures.insert(std::make_pair(friendly_name, tex));

	return tex;

}
