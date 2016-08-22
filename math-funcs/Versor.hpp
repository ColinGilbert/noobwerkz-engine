#pragma once

#include <array>

#include "MathDefines.hpp"

namespace noob
{
	struct vec4;
	struct mat4;
	struct versor
	{

		// And now for the actual class

		versor() {}

		versor(float x, float y, float z, float w)
		{
			q[0] = x;
			q[1] = y;
			q[2] = z;
			q[3] = w;
		}

		versor(const noob::vec4& v)
		{
			q[0] = v[0];
			q[1] = v[1];
			q[2] = v[2];
			q[3] = v[3];
		}

		versor(const btQuaternion& quat)
		{
			q[0] = quat.x();
			q[1] = quat.y();
			q[2] = quat.z();
			q[3] = quat.w();
		}

		versor operator/(float rhs) const
		{
			versor result;
			result.q[0] = q[0] / rhs;
			result.q[1] = q[1] / rhs;
			result.q[2] = q[2] / rhs;
			result.q[3] = q[3] / rhs;
			return result;
		}

		versor operator*(float rhs) const
		{
			versor result;
			result.q[0] = q[0] * rhs;
			result.q[1] = q[1] * rhs;
			result.q[2] = q[2] * rhs;
			result.q[3] = q[3] * rhs;
			return result;
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << ")";
			return w.str();
		}

		versor operator*(const versor& rhs) const
		{
			versor result;
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

		versor operator+(const versor& rhs) const
		{
			versor result;
			result.q[0] = rhs.q[0] + q[0];
			result.q[1] = rhs.q[1] + q[1];
			result.q[2] = rhs.q[2] + q[2];
			result.q[3] = rhs.q[3] + q[3];
			// re-normalize in case of mangling
			return normalize (result);
		}

		versor& operator=(const versor& rhs)
		{
			q[0] = rhs.q[0];
			q[1] = rhs.q[1];
			q[2] = rhs.q[2];
			q[3] = rhs.q[3];
			return *this;
		}

		float& operator[](int x) 
		{
			return q[x];
		}

		std::array<float,4> q;
	};
}
