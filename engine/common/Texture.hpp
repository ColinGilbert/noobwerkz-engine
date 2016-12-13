// These are basically constant struct that have no default contructor but allow copying and assigment.
// These are all friend'ed to the graphics class, in order to ensure that user-programmers don't just make them and pass them about. Doing so would likely cause crash danger.

#pragma once

// std
#include <math.h>

// External libs
#include <noob/component/component.hpp>

// Project-local headers
#include "Attrib.hpp"

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
		R8, RG8, RGB8, SRGB8, RGBA8, SRGBA8, RGB8_COMPRESSED, SRGB8_COMPRESSED, RGB8_A1_COMPRESSED, SRGB8_A1_COMPRESSED, RGBA8_COMPRESSED, SRGBA8_COMPRESSED, INVALID
	};

	// TODO: Move out into math classes.
	static uint32_t get_num_mips(const noob::vec2ui Dims)
	{
		return static_cast<uint32_t>(floor(log2(static_cast<double>(std::max(Dims[0], Dims[1]))))) + 1;
	}

	static uint32_t get_num_mips(const noob::vec3ui Dims)
	{
		return static_cast<uint32_t>(floor(log2(static_cast<double>(std::max(std::max(Dims[0], Dims[1]), Dims[2]))))) + 1;
	}

	struct texture_info
	{
		texture_info() noexcept(true) = default;
		texture_info(bool Mips, noob::pixel_format Pixels) noexcept(true) : mips(Mips), pixels(Pixels) {}
		bool mips; //, sign_normalized; // TODO: Later(?)
		noob::pixel_format pixels;
	};

	// Forward-declare
	class graphics;

	struct texture_1d
	{
		texture_1d() = delete;
		texture_1d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Length) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), length(Length) {}

		texture_1d(const noob::texture_1d& lhs) noexcept(true) = default;
		texture_1d& operator=(const noob::texture_1d& lhs) noexcept(true) = default;

		uint32_t driver_handle;
		const noob::texture_info info;
		const uint32_t length;
	};

	typedef noob::handle<noob::texture_1d> texture_1d_handle;


	struct texture_2d
	{
		texture_2d() = delete;
		texture_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height) noexcept(true) : driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height) {}

		texture_2d(const noob::texture_2d& lhs) noexcept(true) = default;
		texture_2d& operator=(const noob::texture_2d& lhs) noexcept(true) = default;

		const uint32_t driver_handle;
		const noob::texture_info info;
		const uint32_t width, height;
	};

	typedef noob::handle<noob::texture_2d> texture_2d_handle;


	struct texture_array_2d
	{
		texture_array_2d() = delete;
		texture_array_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Indices) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), indices(Indices) {}

		texture_array_2d(const noob::texture_array_2d& lhs) noexcept(true) = default;
		texture_array_2d& operator=(const noob::texture_array_2d& lhs) noexcept(true) = default;

		const uint32_t driver_handle;
		const noob::texture_info info;
		const uint32_t width, height, indices;
	};

	typedef noob::handle<noob::texture_array_2d> texture_array_2d_handle;


	struct texture_3d
	{
		texture_3d() = delete;
		texture_3d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Depth) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), depth(Depth) {}

		texture_3d(const noob::texture_3d& lhs) noexcept(true) = default;
		texture_3d& operator=(const noob::texture_3d& lhs) noexcept(true) = default;

		const uint32_t driver_handle;
		const noob::texture_info info;
		const uint32_t width, height, depth;
	};

	typedef noob::handle<noob::texture_3d> texture_3d_handle;

	static std::string to_string(noob::pixel_format Format)
	{
		switch (Format)
		{
			case(noob::pixel_format::R8):
				{
					return "R8";
				}
			case(noob::pixel_format::RG8):
				{
					return "RG8";
				}
			case(noob::pixel_format::RGB8):
				{
					return "RGG8";
				}
			case(noob::pixel_format::SRGB8):
				{
					return "SRGB8";
				}
			case(noob::pixel_format::RGBA8):
				{
					return "RGBA8";
				}
			case(noob::pixel_format::SRGBA8):
				{
					return "SRGBA8";
				}
			case(noob::pixel_format::RGB8_COMPRESSED):
				{
					return "RGG8_COMPRESSED";
				}
			case(noob::pixel_format::SRGB8_COMPRESSED):
				{
					return "SRGG8_COMPRESSED";
				}
			case(noob::pixel_format::RGB8_A1_COMPRESSED):
				{
					return "RGB8_A1_COMPRESSED";
				}
			case(noob::pixel_format::SRGB8_A1_COMPRESSED):
				{
					return "SRGB8_A1_COMPRESSED";
				}
			case(noob::pixel_format::RGBA8_COMPRESSED):
				{
					return "RGBA8_COMPRESSED";
				}
			case(noob::pixel_format::INVALID):
				{
					return "INVALID";
				}
		};

		return "INVALID";
	}
}
