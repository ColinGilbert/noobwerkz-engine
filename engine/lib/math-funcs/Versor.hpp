#pragma once

#include <array>

#include "Vec4.hpp"

namespace noob
{
	template <typename T>
		struct versor_type
		{
			versor_type() noexcept(true) {}

			versor_type(T x, T y, T z, T w) noexcept(true)
			{
				q[0] = x;
				q[1] = y;
				q[2] = z;
				q[3] = w;
			}

			versor_type operator/(T rhs) const noexcept(true)
			{
				versor_type result;
				result.q[0] = q[0] / rhs;
				result.q[1] = q[1] / rhs;
				result.q[2] = q[2] / rhs;
				result.q[3] = q[3] / rhs;
				return result;
			}

			versor_type operator*(T rhs) const noexcept(true)
			{
				versor_type result;
				result.q[0] = q[0] * rhs;
				result.q[1] = q[1] * rhs;
				result.q[2] = q[2] * rhs;
				result.q[3] = q[3] * rhs;
				return result;
			}

			versor_type operator*(const versor_type& rhs) const noexcept(true)
			{
				versor_type result;
				result.q[0] = rhs.q[0] * q[0] - rhs.q[1] * q[1] -
					rhs.q[2] * q[2] - rhs.q[3] * q[3];
				result.q[1] = rhs.q[0] * q[1] + rhs.q[1] * q[0] -
					rhs.q[2] * q[3] + rhs.q[3] * q[2];
				result.q[2] = rhs.q[0] * q[2] + rhs.q[1] * q[3] +
					rhs.q[2] * q[0] - rhs.q[3] * q[1];
				result.q[3] = rhs.q[0] * q[3] - rhs.q[1] * q[2] +
					rhs.q[2] * q[1] + rhs.q[3] * q[0];
				// re-normalize in case of mangling
				return normalize (result);
			}

			versor_type operator+(const versor_type& rhs) const noexcept(true)
			{
				versor_type result;
				result.q[0] = rhs.q[0] + q[0];
				result.q[1] = rhs.q[1] + q[1];
				result.q[2] = rhs.q[2] + q[2];
				result.q[3] = rhs.q[3] + q[3];
				// re-normalize in case of mangling
				return normalize (result);
			}

			versor_type& operator=(const versor_type& rhs) noexcept(true)
			{
				q[0] = rhs.q[0];
				q[1] = rhs.q[1];
				q[2] = rhs.q[2];
				q[3] = rhs.q[3];
				return *this;
			}

			T& operator[](uint32_t x) noexcept(true)
			{
				return q[x];
			}
			
			std::array<T,4> q;
		};
}
