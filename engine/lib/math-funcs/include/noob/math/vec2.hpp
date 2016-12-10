#pragma once

#include <array>

namespace noob
{
	template <typename T>
		struct vec2_type
		{
			vec2_type() noexcept(true) {}

			vec2_type(T x, T y) noexcept(true)
			{
				v[0] = x;
				v[1] = y;
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return v[x];
			}

			const T& operator[](uint32_t x) const noexcept(true)
			{
				return v[x];
			}

			std::array<T, 2> v;
		};
}
