// Header file we're implementing
#include "TextureLoader.hpp"

// External libs
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#include "stb_image.h"

// Project headers
#include "Logger.hpp"

void log_texload(uint32_t Width, uint32_t Height, uint32_t Channels)
{
	noob::logger::log(noob::importance::INFO, noob::concat("[Loading texture from memory] Got texture data! Width = ", noob::to_string(Width), ", height = ", noob::to_string(Height), ", channels = ", noob::to_string(Channels)));
}


void noob::texture_loader_2d::from_mem(const std::string& Data, bool Compressed) noexcept(true)
{
	int height, width, channels_found;
	height = width = channels_found = 0;
	is_compressed = Compressed;
	internal_buffer = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(Data.c_str()), Data.size(), &height, &width, &channels_found, 0);
	dims[0] = static_cast<uint32_t>(width);
	dims[1] = static_cast<uint32_t>(height);
	if (internal_buffer != nullptr)
	{
		is_valid = true;
		if (channels_found > 0)
		{
			switch (channels_found)
			{
				case (1):
					{
						pixels = noob::pixel_format::R8;
						log_texload(dims[0], dims[1], channels_found);
						break;
					}
				case (2):
					{
						pixels = noob::pixel_format::RG8;
						log_texload(dims[0], dims[1], channels_found);
						break;
					}

				case (3):
					{
						pixels = noob::pixel_format::RGB8;
						log_texload(dims[0], dims[1], channels_found);
						break;
					}
				case (4):
					{
						pixels = noob::pixel_format::RGBA8;
						log_texload(dims[0], dims[1], channels_found);
						break;
					}
				default:
					{
						pixels = noob::pixel_format::INVALID;
						break;
					}
			};

			buf_size = channels_found * height * width;
			is_valid = true;
		}
		else
		{
			noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Failed to load texture due to format issues. Width = ", noob::to_string(dims[0]), ", height = ", noob::to_string(dims[1]), ", channels = ", noob::to_string(channels_found)));
		}
	}
	else // if buffer == nullptr
	{
		noob::logger::log(noob::importance::WARNING, noob::concat("[Loading texture from memory] Failed to even load texture data!"));

	}
}


void noob::texture_loader_2d::from_mem_raw(const noob::vec2ui Dims, bool Mipped, noob::pixel_format PixelFormat, const uint8_t* BufferHead, uint32_t BufferSize) noexcept(true)
{
	// TODO: Replace with proper measurements. These are just *very* limited heuristics.
	if (Dims[0] < 1 || Dims[1] < 1 || BufferSize == 0) 
	{
		is_valid = false;
		buf_size = 0;
		internal_buffer = nullptr;
		return;
	}
	is_valid = true;
	is_mipped = Mipped;
	is_compressed = noob::is_compressed(PixelFormat);
	dims = Dims;
	pixels = PixelFormat;
	internal_buffer = const_cast<uint8_t*>(BufferHead);
	buf_size = BufferSize;
}


bool noob::texture_loader_2d::valid() const noexcept(true)
{
	return is_valid;
}


bool noob::texture_loader_2d::compressed() const noexcept(true)
{
	return is_compressed;
}


bool noob::texture_loader_2d::mips() const noexcept(true)
{
	return is_mipped;
}


noob::vec2ui noob::texture_loader_2d::dimensions() const noexcept(true)
{
	return dims;
}


noob::pixel_format noob::texture_loader_2d::format() const noexcept(true)
{
	return pixels;
}


uint8_t* noob::texture_loader_2d::buffer() const noexcept(true)
{
	return internal_buffer;
}


uint32_t noob::texture_loader_2d::buffer_size() const noexcept(true)
{
	return buf_size;
};


void noob::texture_loader_2d::free() noexcept(true)
{
	if(is_valid)
	{
		stbi_image_free(internal_buffer);
		buf_size = 0;
		is_valid = false;
	}
}
