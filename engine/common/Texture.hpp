#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	struct texture
	{
		enum class storage_type
		{
			TEX_1D, TEX_2D_ARRAY, TEX_3D, TEX_CUBE
		};

		enum class compression_type
		{
			NONE, ETC2, PVRTC, ADST
		};

		uint32_t handle;
		noob::texture::storage_type storage;
		noob::texture::compression_type compression;
	};
	
	typedef noob::handle<noob::texture> texture_handle;
}
