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

		// std::string to_string() const
		// {
		// fmt::MemoryWriter w;
		// w << "(" << q[0] << ", " << q[1] << ", " << q[2] << ", " << q[3] << ")";
		// return w.str();
		// }

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

		float get_opIndex(int i) const
		{
			if (i > 3 ) return q[3];
			if (i < 0) return q[0];
			return q[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 3 && i < 0) return;
			q[i] = value;
		}


		// ---------------- STATICS -----------------------

		static versor normalize(const versor& q)
		{
			// norm(q) = q / magnitude (q)
			// magnitude (q) = sqrt (w*w + x*x...)
			// only compute sqrt if interior sum != 1.0
			versor qq(q);
			float sum = qq.q[0] * qq.q[0] + qq.q[1] * qq.q[1] + qq.q[2] * qq.q[2] + qq.q[3] * qq.q[3];
			// NB: floats have min 6 digits of precision
			const float thresh = 0.0001f;
			if (fabs (1.0f - sum) < thresh)
			{
				return q;
			}
			float mag = sqrt(sum);
			return qq / mag;
		}

		static versor from_axis_rad(float radians, float x, float y, float z)
		{
			versor result;
			result.q[0] = cos (radians / 2.0);
			result.q[1] = sin (radians / 2.0) * x;
			result.q[2] = sin (radians / 2.0) * y;
			result.q[3] = sin (radians / 2.0) * z;
			return result;
		}

		static versor from_axis_deg(float degrees, float x, float y, float z)
		{
			return quat_from_axis_rad (ONE_DEG_IN_RAD * degrees, x, y, z);
		}

		mat4 to_mat4() const
		{
			float w = q[0];
			float x = q[1];
			float y = q[2];
			float z = q[3];
			return mat4 (1.0f - 2.0f * y * y - 2.0f * z * z,
					2.0f * x * y + 2.0f * w * z,
					2.0f * x * z - 2.0f * w * y,
					0.0f,
					2.0f * x * y - 2.0f * w * z,
					1.0f - 2.0f * x * x - 2.0f * z * z,
					2.0f * y * z + 2.0f * w * x,
					0.0f,
					2.0f * x * z + 2.0f * w * y,
					2.0f * y * z - 2.0f * w * x,
					1.0f - 2.0f * x * x - 2.0f * y * y,
					0.0f,
					0.0f,
					0.0f,
					0.0f,
					1.0f
				    );
		}

		static float dot(const versor& q, const versor& r)
		{
			return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
		}

		static versor slerp(const versor& q, const versor& r, float t)
		{
			versor temp_q(q);
			// angle between q0-q1
			float cos_half_theta = dot(temp_q, r);
			// as found here http://stackoverflow.com/questions/2886606/flipping-issue-when-interpolating-rotations-using-quaternions
			// if dot product is negative then one quaternion should be negated, to make
			// it take the short way around, rather than the long way
			// yeah! and furthermore Susan, I had to recalculate the d.p. after this

			if (cos_half_theta < 0.0f)
			{
				for (int i = 0; i < 4; i++)
				{
					temp_q.q[i] *= -1.0f;
				}
				cos_half_theta = dot (temp_q, r);
			}
			// if qa=qb or qa=-qb then theta = 0 and we can return qa
			if (fabs(cos_half_theta) >= 1.0f)
			{
				return temp_q;
			}
			// Calculate temporary values
			float sin_half_theta = sqrt(1.0f - cos_half_theta * cos_half_theta);
			// if theta = 180 degrees then result is not fully defined
			// we could rotate around any axis normal to qa or qb
			versor result;
			if (fabs(sin_half_theta) < 0.001f)
			{
				for (int i = 0; i < 4; i++)
				{
					result.q[i] = (1.0f - t) * temp_q.q[i] + t * r.q[i];
				}
				return result;
			}
			float half_theta = acos(cos_half_theta);
			float a = sin((1.0f - t) * half_theta) / sin_half_theta;
			float b = sin(t * half_theta) / sin_half_theta;
			for (int i = 0; i < 4; i++)
			{
				result.q[i] = temp_q.q[i] * a + r.q[i] * b;
			}
			return result;
		}

		static versor from_mat4(const mat4& m)
		{
			glm::mat4 mm = glm::make_mat4(&m.m[0]);
			glm::quat q = glm::quat_cast(mm);
			noob::versor qq;
			qq.q[0] = q[0];
			qq.q[1] = q[1];
			qq.q[2] = q[2];
			qq.q[3] = q[3];
			return qq;
		}



		std::array<float,4> q;
	};
}
