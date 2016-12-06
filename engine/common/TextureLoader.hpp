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
		~texture_loader_2d() noexcept(true);
		void from_mem(const std::string& Data , bool Compressed) noexcept(true);
		void from_fs(const std::string& Data, bool Compressed) noexcept(true);
	protected:	
		bool valid, compressed;
		std::array<uint32_t, 2> dims;
		noob::pixel_format pixels;
		unsigned char* buffer;
		uint32_t channels;
	};
}
