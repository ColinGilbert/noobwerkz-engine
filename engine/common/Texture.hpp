// These are basically constant struct that have no default contructor but allow copying and assigment.
// These are all friend'ed to the graphics class, in order to ensure that user-programmers don't just make them and pass them about. Doing so would likely cause crash danger.

#pragma once

#include <noob/component/component.hpp>

#include "Attrib.hpp"


namespace noob
{
	/*
	   enum class texture_storage
	   {
	   TEX_1D, TEX_2D, ARRAY_TEX_2D, CUBEMAP, TEX_3D
	   };
	   */
	//uint32_t tex_base_level = 0;

	enum class tex_compare_mode { COMPARE_REF_TO_TEXTURE, NONE };

	enum class tex_compare_func { LEQUAL, GEQUAL, LESS, GREATER, EQUAL, NOTEQUAL, ALWAYS, NEVER };

	enum class tex_min_filter { NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR, LINEAR_MIPMAP_LINEAR };

	enum class tex_mag_filter { GL_NEAREST, GL_LINEAR };

	// int32_t texture_min_lod = -1000;

	// int32_t texture_max_lod = 1000;

	// Passed in arrays of four
	enum class tex_swizzle {RED, GREEN, BLUE, ALPHA, ZERO, ONE};

	// Pass an array of three of these
	enum class tex_wrap_mode { CLAMP_TO_EDGE, MIRRORED_REPEAT, REPEAT };

	enum class texture_channels
	{
		R, RG, RGB, RGBA, DEPTH, DEPTH_STENCIL
	};

	enum class scalar_type
	{
		UINT, INT, FLOAT
	};

	struct texture_info
	{
		noob::texture_channels channels;
		std::array<noob::scalar_type, 4> channel_scalars;		
		std::array<uint8_t, 4> channel_bits;
		bool compressed, mips, s_normalized;
	};

	class graphics;

	struct texture_1d
	{
		friend class graphics;

		private:
		const uint32_t driver_handle;

		texture_1d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Length) noexcept(true) : driver_handle(DriverHandle), info(TexInfo), length(Length) {}
		texture_1d() = delete;

		public:
		texture_1d(const noob::texture_1d& lhs) noexcept(true) = default;
		texture_1d& operator=(const noob::texture_1d& lhs) noexcept(true) = default;

		const texture_info info;
		const uint32_t length;
	};

	typedef noob::handle<noob::texture_1d> texture_1d_handle;

	struct texture_2d
	{
		friend class graphics;

		private:
		const uint32_t driver_handle;

		texture_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height) {}

		public:		
		texture_2d() = delete;
		texture_2d(const noob::texture_2d& lhs) noexcept(true) = default;
		texture_2d& operator=(const noob::texture_2d& lhs) noexcept(true) = default;

		const texture_info info;
		const uint32_t width, height;
	};

	typedef noob::handle<noob::texture_2d> texture_2d_handle;

	struct texture_array_2d
	{
		friend class graphics;

		public:

		private:
		const uint32_t driver_handle;

		texture_array_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Indices) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), indices(Indices) {}

		public:		
		texture_array_2d() = delete;
		texture_array_2d(const noob::texture_array_2d& lhs) noexcept(true) = default;
		texture_array_2d& operator=(const noob::texture_array_2d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t width, height, indices;
	};

	typedef noob::handle<noob::texture_array_2d> texture_array_2d_handle;


	struct texture_3d
	{
		friend class graphics;

		public:
		private:
		const uint32_t driver_handle;

		texture_3d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Width, uint32_t Height, uint32_t Depth) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), width(Width), height(Height), depth(Depth) {}

		public:		
		texture_3d() = delete;
		texture_3d(const noob::texture_3d& lhs) noexcept(true) = default;
		texture_3d& operator=(const noob::texture_3d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const uint32_t width, height, depth;
	};

	typedef noob::handle<noob::texture_3d> texture_3d_handle;

}
