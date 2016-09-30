#pragma once

#include <array>

#include "Vec3.hpp"

namespace noob
{
	template <typename T>
		struct vec4_type
		{
			vec4_type() noexcept(true) {}

			vec4_type(T x, T y, T z, T w) noexcept(true)
			{
				v[0] = x;
				v[1] = y;
				v[2] = z;
				v[3] = w;
			}

			vec4_type(const vec3_type<T>& arg, T w) noexcept(true)
			{
				v[0] = arg.v[0];
				v[1] = arg.v[1];
				v[2] = arg.v[2];
				v[3] = w;
			}

			vec4_type operator+(const vec4_type& rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] + rhs.v[0];
				vc.v[1] = v[1] + rhs.v[1];
				vc.v[2] = v[2] + rhs.v[2];
				vc.v[3] = v[3] + rhs.v[3];
				return vc;
			}

			vec4_type& operator+=(const vec4_type& rhs) noexcept(true)
			{
				v[0] += rhs.v[0];
				v[1] += rhs.v[1];
				v[2] += rhs.v[2];
				v[3] += rhs.v[3];
				return *this; // return self
			}

			vec4_type operator-(const vec4_type& rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] - rhs.v[0];
				vc.v[1] = v[1] - rhs.v[1];
				vc.v[2] = v[2] - rhs.v[2];
				vc.v[3] = v[3] - rhs.v[3];
				return vc;
			}

			vec4_type& operator-=(const vec4_type& rhs) noexcept(true)
			{
				v[0] -= rhs.v[0];
				v[1] -= rhs.v[1];
				v[2] -= rhs.v[2];
				v[3] -= rhs.v[3];
				return *this;
			}

			vec4_type operator+(T rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] + rhs;
				vc.v[1] = v[1] + rhs;
				vc.v[2] = v[2] + rhs;
				vc.v[3] = v[3] + rhs;
				return vc;
			}

			vec4_type operator-(T rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] - rhs;
				vc.v[1] = v[1] - rhs;
				vc.v[2] = v[2] - rhs;
				vc.v[3] = v[3] - rhs;
				return vc;
			}

			vec4_type operator*(T rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] * rhs;
				vc.v[1] = v[1] * rhs;
				vc.v[2] = v[2] * rhs;
				vc.v[3] = v[3] * rhs;
				return vc;
			}

			vec4_type operator/(T rhs) const noexcept(true)
			{
				vec4_type vc;
				vc.v[0] = v[0] / rhs;
				vc.v[1] = v[1] / rhs;
				vc.v[2] = v[2] / rhs;
				vc.v[3] = v[3] / rhs;
				return vc;
			}

			vec4_type& operator*=(T rhs) noexcept(true)
			{
				v[0] = v[0] * rhs;
				v[1] = v[1] * rhs;
				v[2] = v[2] * rhs;
				v[3] = v[3] * rhs;
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
			
			std::array<T, 4> v;
		};

}
