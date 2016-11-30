#pragma once

#include <noob/component/component.hpp>

#include "Attrib.hpp"


namespace noob
{
	enum class texture_storage
	{
		TEX_1D, TEX_2D, ARRAY_TEX_2D, CUBEMAP, TEX_3D
	};

	enum class texture_channels
	{
		R, RG, RGBA
	};

	class graphics;

	struct texture
	{
		friend class graphics;
		// Your earnest, friendly massive constructor used to enforce proper object creation. The graphics class gives these opaquely to the enduser.
		texture(uint32_t DriverHandle, noob::texture_storage Storage, noob::texture_channels Channels, noob::attrib::unit_type BitDepth, uint32_t DimX, uint32_t DimY, uint32_t DimZ, bool Compressed, bool Mips) noexcept(true) :  driver_handle(DriverHandle), storage(Storage), channels(Channels), bit_depth(BitDepth), x(DimX), y(DimY), z(DimZ), compressed(Compressed), mips(Mips) {}
		
		
		texture() = delete;
		texture(const noob::texture& lhs) noexcept(true) = default;
		texture& operator=(const noob::texture& lhs) noexcept(true) = default;

		private:
		const uint32_t driver_handle;

		public:
		const noob::texture_storage storage;
		const noob::texture_channels channels;
		const noob::attrib::unit_type bit_depth;
		const uint32_t x, y, z;
		const bool compressed;
		const bool mips;
	};

	typedef noob::handle<noob::texture> texture_handle;
}
