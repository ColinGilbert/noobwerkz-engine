#pragma once

#include <array>

// #include <rdestl/fixed_array.h>

namespace noob
{
	template <typename T>
		struct vec3_type
		{
			vec3_type() noexcept(true) {}

			vec3_type(T x, T y, T z) noexcept(true)
			{
				v[0] = x;
				v[1] = y;
				v[2] = z;
			}

			vec3_type(const vec3_type& vv) noexcept(true)
			{
				v[0] = vv.v[0];
				v[1] = vv.v[1];
				v[2] = vv.v[2];
			}

			vec3_type operator+(const vec3_type& rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] + rhs.v[0];
				vc.v[1] = v[1] + rhs.v[1];
				vc.v[2] = v[2] + rhs.v[2];
				return vc;
			}

			vec3_type& operator+=(const vec3_type& rhs) noexcept(true)
			{
				v[0] += rhs.v[0];
				v[1] += rhs.v[1];
				v[2] += rhs.v[2];
				return *this; // return self
			}

			vec3_type operator-(const vec3_type& rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] - rhs.v[0];
				vc.v[1] = v[1] - rhs.v[1];
				vc.v[2] = v[2] - rhs.v[2];
				return vc;
			}

			vec3_type& operator-=(const vec3_type& rhs) noexcept(true)
			{
				v[0] -= rhs.v[0];
				v[1] -= rhs.v[1];
				v[2] -= rhs.v[2];
				return *this;
			}

			vec3_type operator+(T rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] + rhs;
				vc.v[1] = v[1] + rhs;
				vc.v[2] = v[2] + rhs;
				return vc;
			}

			vec3_type operator-(T rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] - rhs;
				vc.v[1] = v[1] - rhs;
				vc.v[2] = v[2] - rhs;
				return vc;
			}

			vec3_type operator*(T rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] * rhs;
				vc.v[1] = v[1] * rhs;
				vc.v[2] = v[2] * rhs;
				return vc;
			}

			vec3_type operator/(T rhs) const noexcept(true)
			{
				vec3_type vc;
				vc.v[0] = v[0] / rhs;
				vc.v[1] = v[1] / rhs;
				vc.v[2] = v[2] / rhs;
				return vc;
			}

			vec3_type& operator*=(T rhs) noexcept(true)
			{
				v[0] = v[0] * rhs;
				v[1] = v[1] * rhs;
				v[2] = v[2] * rhs;
				return *this;
			}

			vec3_type& operator=(const vec3_type& rhs) noexcept(true)
			{
				v[0] = rhs.v[0];
				v[1] = rhs.v[1];
				v[2] = rhs.v[2];
				return *this;
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return v[x];
			}

			const T& operator[](uint32_t x) const noexcept(true)
			{
				return v[x];
			}

			std::array<T, 3> v;
		};
}
