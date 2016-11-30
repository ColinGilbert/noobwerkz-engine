#pragma once

#include <noob/component/component.hpp>

namespace noob
{
	class texture
	{
		public:

		enum class storage_type
		{
			TEX_1D, TEX_2D, ARRAY_TEX_2D, CUBEMAP, TEX_3D
		};

		enum class channel_type
		{
			R, RG, RGBA
		};

		enum class colour_depth_type
		{
			INT8, UINT8, INT16, UINT16, INT32, HALF_FLOAT, FLOAT
		};
		
		noob::texture::storage_type storage() const noexcept(true)
		{
			return m_storage;
		}

		noob::texture::channel_type channels() const noexcept(true)
		{
			return m_channels;
		}

		noob::texture::colour_depth_type colour_depth() const noexcept(true)
		{
			return m_colour_depth;
		}

		protected:
		noob::texture::storage_type m_storage;
		noob::texture::channel_type m_channels;
		noob::texture::colour_depth_type m_colour_depth;
		uint32_t handle, m_width, m_height, m_depth;
		bool m_compressed, m_mips;
	};

	typedef noob::handle<noob::texture> texture_handle;
}
