#pragma once

#include <array>

#include "Vec4.hpp"

namespace noob
{
	/* stored like this:
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15 */
	template <typename T>
		struct mat4_type
		{
			mat4_type() noexcept(true) {}

			mat4_type(T a, T b, T c, T d, T e, T f, T g, T h, T i, T j, T k, T l, T mm, T n, T o, T p) noexcept(true)
			{
				m[0] = a;
				m[1] = b;
				m[2] = c;
				m[3] = d;
				m[4] = e;
				m[5] = f;
				m[6] = g;
				m[7] = h;
				m[8] = i;
				m[9] = j;
				m[10] = k;
				m[11] = l;
				m[12] = mm;
				m[13] = n;
				m[14] = o;
				m[15] = p;
			}

			mat4_type(const std::array<T,16>& mm) noexcept(true)
			{
				for (uint32_t i = 0; i < 16; i++)
				{
					m[i] = mm[i];
				}
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return m[x];
			}

			vec4_type<T> operator*(const vec4_type<T>& rhs) const noexcept(true)
			{
				// 0x + 4y + 8z + 12w
				T x = m[0] * rhs.v[0] +
					m[4] * rhs.v[1] +
					m[8] * rhs.v[2] +
					m[12] * rhs.v[3];
				// 1x + 5y + 9z + 13w
				T y = m[1] * rhs.v[0] +
					m[5] * rhs.v[1] +
					m[9] * rhs.v[2] +
					m[13] * rhs.v[3];
				// 2x + 6y + 10z + 14w
				T z = m[2] * rhs.v[0] +
					m[6] * rhs.v[1] +
					m[10] * rhs.v[2] +
					m[14] * rhs.v[3];
				// 3x + 7y + 11z + 15w
				T w = m[3] * rhs.v[0] +
					m[7] * rhs.v[1] +
					m[11] * rhs.v[2] +
					m[15] * rhs.v[3];
				return vec4_type<T>(x, y, z, w);
			}

			mat4_type operator*(const mat4_type& rhs) const noexcept(true)
			{
				mat4_type r;
				std::fill_n(&r.m[0], 16, 0.0);

				uint32_t r_index = 0;
				for (uint32_t col = 0; col < 4; col++)
				{
					for (uint32_t row = 0; row < 4; row++) 
					{
						T sum = 0.0f;
						for (uint32_t i = 0; i < 4; i++) 
						{
							sum += rhs.m[i + col * 4] * m[row + i * 4];
						}
						r.m[r_index] = sum;
						r_index++;
					}
				}
				return r;
			}

			mat4_type& operator=(const mat4_type& rhs) noexcept(true)
			{
				for (uint32_t i = 0; i < 16; i++)
				{
					m[i] = rhs.m[i];
				}
				return *this;
			}

			std::array<T, 16> m;
		};
}
