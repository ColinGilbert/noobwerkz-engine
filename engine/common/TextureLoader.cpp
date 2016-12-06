// Header file we're implementing
#include "TextureLoader.hpp"

// External libs
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

// Project headers
#include "NoobUtils.hpp"

void log_texload(uint32_t Width, uint32_t Height, uint32_t Channels)
{
	noob::logger::log(noob::importance::INFO, noob::concat("[Loading texture from memory] Got texture data! Width = ", noob::to_string(Width), ", height = ", noob::to_string(Height), ", channels = ", noob::to_string(Channels)));
}

noob::loaded_texture_data_2d noob::load_texture_mem(const std::string& Data) noexcept(true)
{
	int width, height, channels;
	unsigned char* buffer = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(Data.c_str()), Data.size(), &width, &height, &channels, 0);
	// rgb is now three bytes per pixel, width*height size. Or NULL if load failed. Do something with it...
	if (buffer != nullptr)
	{
		if (channels > 0 && width > 0 && height > 0)
		{
			switch (channels)
			{
				case (1):
					{
						log_texload(width, height, channels);
						return noob::loaded_texture_data_2d(true, false, width, height, noob::pixel_format::R8);
					}
				case (2):
					{
						log_texload(width, height, channels);
						return noob::loaded_texture_data_2d(true, false, width, height, noob::pixel_format::RG8);
					}

				case (3):
					{
						log_texload(width, height, channels);
						return noob::loaded_texture_data_2d(true, false, width, height, noob::pixel_format::RGB8);
					}

				case (4):
					{
						log_texload(width, height, channels);
						return noob::loaded_texture_data_2d(true, false, width, height, noob::pixel_format::RGBA8);
					}
				default:
					{
						break;
					}
			};

		}
		noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Could not successfully load texture due to format issues. Width = ", noob::to_string(width), ", height = ", noob::to_string(height), ", channels = ", noob::to_string(channels)));
		stbi_image_free(buffer);
	}
	else // if buffer == nullptr
	{
		noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Could not even load texture data!"));

	}

	// Base case.
	return noob::loaded_texture_data_2d(false, false, 0, 0, noob::pixel_format::INVALID);
}
