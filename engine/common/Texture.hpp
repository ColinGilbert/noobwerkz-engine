// These are basically constant struct that have no default contructor but allow copying and assigment.
// These are all friend'ed to the graphics class, in order to ensure that user-programmers don't just make them and pass them about. Doing so would likely cause crash danger.

#pragma once

#include <noob/component/component.hpp>

#include "Attrib.hpp"

#include <math.h>

namespace noob
{
	enum class tex_compare_mode { COMPARE_REF_TO_TEXTURE, NONE };

	enum class tex_compare_func { LEQUAL, GEQUAL, LESS, GREATER, EQUAL, NOTEQUAL, ALWAYS, NEVER };

	enum class tex_min_filter { NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR };

	enum class tex_mag_filter { NEAREST, LINEAR };

	// Passed in arrays of four
	enum class tex_swizzle {RED, GREEN, BLUE, ALPHA, ZERO, ONE};

	// Pass an array of three of these
	enum class tex_wrap_mode { CLAMP_TO_EDGE, MIRRORED_REPEAT, REPEAT };

	enum class pixel_format
	{
		R8, RG8, RGB8, SRGB8, RGBA8, SRGBA8, RGB8_COMPRESSED, SRGB8_COMPRESSED, RGB8_A1_COMPRESSED, SRGB8_A1_COMPRESSED, RGBA8_COMPRESSED, SRGBA8_COMPRESSED
	};

	// TODO: Move out into math classes.
	// TODO: Test
	static uint32_t next_pow2(uint32_t arg)
	{
		int64_t val = arg;
		--val;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		val |= val >> 32;
		++val;
		return static_cast<uint32_t>(val);
	}

	// TODO: Move out into math classes.
	static uint32_t get_num_mips(uint32_t Width, uint32_t Height)
	{
		return static_cast<uint32_t>(floor(log2(static_cast<double>(std::max(Width, Height))))) + 1;
	}

	static uint32_t get_num_mips(uint32_t Width, uint32_t Height, uint32_t Depth)
	{
		return static_cast<uint32_t>(floor(log2(static_cast<double>(std::max(std::max(Width, Height), Depth))))) + 1;
	}

// Per-channel:
// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_HALF_FLOAT, GL_FLOAT
// Per-pixel:
// GL_UNSIGNED_SHORT_5_6_5, GL_UNSIGNED_SHORT_4_4_4_4, GL_UNSIGNED_SHORT_5_5_5_1, GL_UNSIGNED_INT_2_10_10_10_REV, GL_UNSIGNED_INT_10F_11F_11F_REV, GL_UNSIGNED_INT_5_9_9_9_REV, GL_UNSIGNED_INT_24_8, and GL_FLOAT_32_UNSIGNED_INT_24_8_REV.
// Per-pixel, compressed:
// GL_COMPRESSED_RGB8_ETC2, GL_COMPRESSED_SRGB8_ETC2, GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2, GL_COMPRESSED_RGBA8_ETC2_EAC, GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC

	struct texture_info
	{
		bool mips; //, sign_normalized; // TODO: Later(?)
		noob::pixel_format pixels;
	};

	class graphics;

	struct texture_1d
	{
		const uint32_t driver_handle;

		texture_1d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Length) noexcept(true) : driver_handle(DriverHandle), info(TexInfo), length(Length) {}
		texture_1d() = delete;

		texture_1d(const noob::texture_1d& lhs) noexcept(true) = default;
		texture_1d& operator=(const noob::texture_1d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t length;
	};

	typedef noob::handle<noob::texture_1d> texture_1d_handle;

	struct texture_2d
	{
		const uint32_t driver_handle;

		texture_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height) {}

		texture_2d() = delete;
		texture_2d(const noob::texture_2d& lhs) noexcept(true) = default;
		texture_2d& operator=(const noob::texture_2d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t width, height;
	};

	typedef noob::handle<noob::texture_2d> texture_2d_handle;

	struct texture_array_2d
	{
		const uint32_t driver_handle;

		texture_array_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Indices) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), indices(Indices) {}

		texture_array_2d() = delete;
		texture_array_2d(const noob::texture_array_2d& lhs) noexcept(true) = default;
		texture_array_2d& operator=(const noob::texture_array_2d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t width, height, indices;
	};

	typedef noob::handle<noob::texture_array_2d> texture_array_2d_handle;


	struct texture_3d
	{
		const uint32_t driver_handle;

		texture_3d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Depth) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), depth(Depth) {}

		texture_3d() = delete;
		texture_3d(const noob::texture_3d& lhs) noexcept(true) = default;
		texture_3d& operator=(const noob::texture_3d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t width, height, depth;
	};

	typedef noob::handle<noob::texture_3d> texture_3d_handle;

}
