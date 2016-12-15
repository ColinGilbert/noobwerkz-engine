// This class helps you load a texture from file or memory. It handles the unpleasantness of parsing the files yourself. :)
#pragma once

// std
#include <array>

// External libs
#include <noob/math/math_funcs.hpp>

// Internal project headers
#include "Texture.hpp"


namespace noob
{
	class texture_loader_2d
	{
		public:
			texture_loader_2d() noexcept(true) : is_valid(false), is_compressed(false), dims({0,0}), pixels(noob::pixel_format::INVALID), internal_buffer(nullptr), buf_size(0), is_mipped(false) {}

			// These use stb_image
			void from_mem(const std::string& Data, bool Compressed) noexcept(true);
			void from_fs(const std::string& Data, bool Compressed) noexcept(true);
			// This one is for when you need to make a texture manually.
			void from_mem_raw(const noob::vec2ui Dims, bool Mipped, noob::pixel_format PixelFormat, const uint8_t* Buffer) noexcept(true);

			bool valid() const noexcept(true);
			bool compressed() const noexcept(true);
			bool mips() const noexcept(true);		
			noob::vec2ui dimensions() const noexcept(true);
			noob::pixel_format format() const noexcept(true);
			uint8_t* buffer() const noexcept(true);
			uint32_t buffer_size() const noexcept(true);
			void free() noexcept(true);

		protected:	
			bool is_valid, is_compressed;
			noob::vec2ui dims;
			noob::pixel_format pixels;
			uint8_t* internal_buffer;
			uint32_t buf_size;
			bool is_mipped;
	};
}
