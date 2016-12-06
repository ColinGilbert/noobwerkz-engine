#pragma once

// std
#include <array>

// Internal project headers
#include "Texture.hpp"


namespace noob
{
	struct loaded_texture_data_2d
	{
		loaded_texture_data_2d() = delete;
		loaded_texture_data_2d(bool Valid, bool Compressed, uint32_t Width, uint32_t Height, noob::pixel_format Pixels) noexcept(true) : valid(Valid), compressed(Compressed), height(Height), width(Width), pixels(Pixels) {}
		
		loaded_texture_data_2d(const noob::loaded_texture_data_2d&) noexcept(true) = default;
		loaded_texture_data_2d& operator=(const noob::loaded_texture_data_2d&) noexcept(true) = default;
		
		const bool valid, compressed;
		const uint32_t width, height;
		const noob::pixel_format pixels;
	};

	noob::loaded_texture_data_2d load_texture_mem(const std::string&) noexcept(true);
	noob::loaded_texture_data_2d load_texture_fs(const std::string&) noexcept(true);
}
