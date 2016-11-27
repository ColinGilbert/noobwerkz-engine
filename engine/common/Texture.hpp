#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	struct texture
	{
		enum class storage_type
		{
			TEX_2D, ARRAY_TEX_2D, CUBEMAP, TEX_3D
		};

		enum class compression_type
		{
			NONE, ETC2, PVRTC, ADST
		};

		enum class format_type
		{
			RED, RED_INTEGER, RG, RG_INTEGER, RGB, RGB_INTEGER, RGBA, RGBA_INTEGER, DEPTH_COMPONENT, DEPTH_STENCIL, LUMINANCE_ALPHA, LUMINANCE, ALPHA
		};

		enum class numeric_type
		{
			UNSIGNED_BYTE, BYTE, UNSIGNED_SHORT, SHORT, UNSIGNED_INT, INT, HALF_FLOAT, FLOAT
		};

		noob::texture::storage_type storage;
		noob::texture::compression_type compression;
		noob::texture::format_type format;
		noob::texture::numeric_type numeric;
		uint32_t handle;
		uint8_t r_depth, b_depth, g_depth, a_depth;
		bool mips;
	};

	typedef noob::handle<noob::texture> texture_handle;
}
