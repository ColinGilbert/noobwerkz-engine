#pragma once

#include <tuple>
#include <cstdint>

namespace noob
{
	// TODO:
	// static uint32_t pack_8_to_32(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
	// {
	// }

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
	
	// http://stackoverflow.com/questions/1322510/given-an-integer-how-do-i-find-the-next-largest-power-of-two-using-bit-twiddlin
	static uint32_t next_power_of_two(uint32_t arg) noexcept(true)
	{
		uint32_t n = arg;

		n--;
		n |= n >> 1;   // Divide by 2^k for consecutive doublings of k up to 32,
		n |= n >> 2;   // and then or the results.
		n |= n >> 4;
		n |= n >> 8;
		n |= n >> 16;
		n++;

		return n;
	}
}
