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

			vec2_type(const vec2_type& vv) noexcept(true)
			{
				v[0] = vv.v[0];
				v[1] = vv.v[1];
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return v[x];
			}

			const T& operator[](uint32_t x) const noexcept(true)
			{
				return v[x];
			}

			vec2_type operator+(const vec2_type& rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] + rhs[0];
				vc.v[1] = v[1] + rhs[1];
				return vc;
			}

			vec2_type& operator+=(const vec2_type& rhs) noexcept(true)
			{
				v[0] += rhs.v[0];
				v[1] += rhs.v[1];
				return *this; // return self
			}

			vec2_type operator-(const vec2_type& rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] - rhs.v[0];
				vc.v[1] = v[1] - rhs.v[1];
				return vc;
			}

			vec2_type& operator-=(const vec2_type& rhs) noexcept(true)
			{
				v[0] -= rhs.v[0];
				v[1] -= rhs.v[1];
				return *this;
			}

			vec2_type operator+(T rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] + rhs;
				vc.v[1] = v[1] + rhs;
				return vc;
			}

			vec2_type operator-(T rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] - rhs;
				vc.v[1] = v[1] - rhs;
				return vc;
			}

			vec2_type operator*(T rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] * rhs;
				vc.v[1] = v[1] * rhs;
				return vc;
			}

			vec2_type operator/(T rhs) const noexcept(true)
			{
				vec2_type vc;
				vc.v[0] = v[0] / rhs;
				vc.v[1] = v[1] / rhs;
				return vc;
			}

			vec2_type& operator*=(T rhs) noexcept(true)
			{
				v[0] = v[0] * rhs;
				v[1] = v[1] * rhs;
				return *this;
			}


			std::array<T, 2> v;
		};
}
