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
		R, RG, RBG, RGBA
	};

	struct texture_info
	{
		noob::texture_channels channels;
		noob::attrib::unit_type bit_depth;
		bool compressed;
		bool mips;
	};

	class graphics;

	struct texture_1d
	{
		friend class graphics;
		private:
		const uint32_t driver_handle;

		texture_1d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Length) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), length(Length) {}
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

		texture_2d(uint32_t DriverHandle, noob::texture_info TexInfo, uint32_t Length, uint32_t Width) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), length(Length), width(Width) {}

		public:		
		texture_2d() = delete;
		texture_2d(const noob::texture_2d& lhs) noexcept(true) = default;
		texture_2d& operator=(const noob::texture_2d& lhs) noexcept(true) = default;

		const texture_info info;
		const uint32_t length, width;
	};

	typedef noob::handle<noob::texture_2d> texture_2d_handle;

	struct texture_3d
	{
		friend class graphics;
		public:
		enum class treated_as { ARRAY_TEX_2D, TEX_3D };
		private:
		const uint32_t driver_handle;

		texture_3d(uint32_t DriverHandle, noob::texture_info TexInfo, noob::texture_3d::treated_as Treatment, uint32_t Length, uint32_t Width, uint32_t Depth, bool Compressed, bool Mips) noexcept(true) :  driver_handle(DriverHandle), info(TexInfo), treatment(Treatment), length(Length), width(Width), depth(Depth) {}

		public:		
		texture_3d() = delete;
		texture_3d(const noob::texture_3d& lhs) noexcept(true) = default;
		texture_3d& operator=(const noob::texture_3d& lhs) noexcept(true) = default;

		const noob::texture_info info;
		const noob::texture_3d::treated_as treatment;
		const uint32_t length, width, depth;
	};

	typedef noob::handle<noob::texture_3d> texture_3d_handle;

}
