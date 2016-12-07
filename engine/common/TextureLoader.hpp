// This class helps you load a texture from file or memory. It handles the unpleasantness of parsing the files yourself. :)
#pragma once

// std
#include <array>

// Internal project headers
#include "Texture.hpp"


namespace noob
{
	class texture_loader_2d
	{
	public:
		texture_loader_2d() noexcept(true) : is_valid(false), is_compressed(false), dims({0,0}), pixels(noob::pixel_format::INVALID), internal_buffer(nullptr) {}
		~texture_loader_2d() noexcept(true);
		void from_mem(const std::string& Data, bool Compressed) noexcept(true);
		void from_fs(const std::string& Data, bool Compressed) noexcept(true);

		bool valid() const noexcept(true);
		bool compressed() const noexcept(true);
		std::array<uint32_t, 2> dimensions() const noexcept(true);
		noob::pixel_format format() const noexcept(true);
		unsigned char* buffer() const noexcept(true);

	protected:	
		bool is_valid, is_compressed;
		std::array<uint32_t, 2> dims;
		noob::pixel_format pixels;
		unsigned char* internal_buffer;
	};
}
