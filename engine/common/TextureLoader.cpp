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

noob::texture_loader_2d::~texture_loader_2d() noexcept(true)
{
	if(valid)
	{
		stbi_image_free(buffer);
	}
}

void noob::texture_loader_2d::from_mem(const std::string& Data, bool Compressed) noexcept(true)
{
	int height, width, chans;
	height = width = chans = 0;
	compressed = Compressed;
	buffer = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(Data.c_str()), Data.size(), &height, &width, &chans, 0);
	dims[0] = static_cast<uint32_t>(width);
	dims[1] = static_cast<uint32_t>(height);
	channels = static_cast<uint32_t>(chans);
	if (buffer != nullptr)
	{
		valid = true;
		if (dims[0] > 0 && dims[1] > 0 && channels > 0)
		{
			switch (channels)
			{
				case (1):
					{
						pixels = noob::pixel_format::R8;
						log_texload(dims[0], dims[1], channels);
					}
				case (2):
					{
						pixels = noob::pixel_format::RG8;
						log_texload(dims[0], dims[1], channels);
					}

				case (3):
					{
						pixels = noob::pixel_format::RGB8;
						log_texload(dims[0], dims[1], channels);
					}

				case (4):
					{
						pixels = noob::pixel_format::RGBA8;
						log_texload(dims[0], dims[1], channels);
					}
				default:
					{
						pixels = noob::pixel_format::INVALID;
						break;
					}
			};

		}
		else
		{
			noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Could not successfully load texture due to format issues. Width = ", noob::to_string(dims[0]), ", height = ", noob::to_string(dims[1]), ", channels = ", noob::to_string(channels)));
		}
	}
	else // if buffer == nullptr
	{
		noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Could not even load texture data!"));

	}

	// Base case.
	valid = false;
}

