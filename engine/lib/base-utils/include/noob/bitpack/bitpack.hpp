#pragma once

#include <tuple>
#include <cstdint>

namespace noob
{
	static uint64_t pack_32_to_64(uint32_t x, uint32_t y) noexcept(true)
	{
		return static_cast<uint64_t>(x) << 32 | y;
	}

	static std::tuple<uint32_t, uint32_t> pack_64_to_32(uint64_t arg) noexcept(true)
	{
		uint32_t x = static_cast<uint32_t>(arg >> 32);
		uint32_t y = static_cast<uint32_t>(arg);
		return std::make_tuple(x,y);
	}
}
