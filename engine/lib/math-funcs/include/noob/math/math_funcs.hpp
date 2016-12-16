// TODO: Convert to Eigen and test
#pragma once

#include <array>
#include <cmath>

#define GLM_FORCE_CXX98
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <btBulletDynamicsCommon.h>
#include <Eigen/Geometry>

#define NOOB_PI 3.1415926535
#define NOOB_TAU 2.0 * NOOB_PI
#define NOOB_ONE_DEG_IN_RAD (2.0 * NOOB_PI) / 360.0 // 0.017444444
#define NOOB_ONE_RAD_IN_DEG 360.0 / (2.0 * NOOB_PI) //57.2957795
#define NOOB_TWO_PI (2.0 * NOOB_PI)
#define NOOB_EPSILON 0.0001

#include "vec2.hpp"
#include "vec3.hpp"
#include "vec3.hpp"
#include "vec4.hpp"
#include "versor.hpp"
#include "mat3.hpp"
#include "mat4.hpp"
#include "plane.hpp"
#include "bbox.hpp"

namespace noob
{
	typedef vec2_type<float> vec2f;
	typedef vec3_type<float> vec3f;
	typedef vec4_type<float> vec4f;
	typedef versor_type<float> versorf;
	typedef mat3_type<float> mat3f;
	typedef mat4_type<float> mat4f;

	typedef vec2_type<double> vec2d;
	typedef vec3_type<double> vec3d;
	typedef vec4_type<double> vec4d;
	typedef versor_type<double> versord;
	typedef mat3_type<double> mat3d;
	typedef mat4_type<double> mat4d;

	typedef vec2_type<uint32_t> vec2ui;
	typedef vec3_type<uint32_t> vec3ui;
	typedef vec4_type<uint32_t> vec4ui;
	typedef versor_type<uint32_t> versorui;
	typedef mat3_type<uint32_t> mat3ui;
	typedef mat4_type<uint32_t> mat4ui;



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UTILITY TYPES:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	enum class binary_op
	{
		ADD, SUBTRACT
	};

	enum class csg_op
	{
		UNION = 0, DIFFERENCE = 1, INTERSECTION = 2
	};

	enum class cardinal_axis
	{
		X, Y, Z
	};

	static uint32_t next_pow2(uint32_t arg)
	{
		uint32_t val = arg;
		--val;
		val |= val >> 1;
		val |= val >> 2;
		val |= val >> 4;
		val |= val >> 8;
		val |= val >> 16;
		++val;
		return val;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ONE-FLOAT FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO: Test

	template <typename T> int sign(T val) noexcept(true)
	{
		return (T(0) < val) - (val < T(0));
	}

	static bool approximately_zero(float a) noexcept(true)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a) <= NOOB_EPSILON) return false;
		else return true;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TWO-FLOAT FUNCTIONS(S):
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	static bool compare_floats(float a, float b) noexcept(true)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a - b) <= NOOB_EPSILON * std::fabs(a)) return false;
		else return true;
	}


	// ENSURING DOUBLE-POINT PRECISION
	template <typename First, typename Second> 
		double div_dp(First first, Second second)
		{
			return static_cast<double>(first) / static_cast<double>(second);
		}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CONVERSION UTILITY FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
		static vec3_type<T> vec3_from_vec4(const vec4_type<T>& vv) noexcept(true)
		{
			noob::vec3_type<T> v;
			v.v[0] = vv.v[0];
			v.v[1] = vv.v[1];
			v.v[2] = vv.v[2];
			return v;
		}

	template <typename T>
		static vec3_type<T> vec3_from_array(const std::array<T, 3>& a) noexcept(true)
		{
			noob::vec3_type<T> v;
			v.v[0] = a[0];
			v.v[1] = a[1];
			v.v[2] = a[2];
			return v;
		}

	static vec3f vec3f_from_bullet(const btVector3& btVec) noexcept(true)
	{
		noob::vec3f v;
		v.v[0] = btVec[0];
		v.v[1] = btVec[1];
		v.v[2] = btVec[2];
		return v;
	}

	static vec3f vec3f_from_eigen(const Eigen::Vector3f& p) noexcept(true)
	{
		noob::vec3f v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	// Whatever the hell you gotta do to compile sometimes, man...
	static vec3f vec3f_from_eigen_block(const Eigen::Block<const Eigen::Matrix<float, 4, 1>, 3, 1, false> n) noexcept(true)
	{
		noob::vec3f v;
		v.v[0] = n[0];
		v.v[1] = n[1];
		v.v[2] = n[2];
		return v;
	}

	static versorf versorf_from_bullet(const btQuaternion& arg) noexcept(true)
	{
		noob::versorf qq;
		qq.q[0] = arg[0];
		qq.q[1] = arg[1];
		qq.q[2] = arg[2];
		qq.q[3] = arg[3];
		return qq;
	}

	static versorf versorf_from_eigen(const Eigen::Quaternion<float>& arg) noexcept(true)
	{
		noob::versorf qq;
		qq.q[0] = arg.x();
		qq.q[1] = arg.y();
		qq.q[2] = arg.z();
		qq.q[3] = arg.w();
		return qq;
	}

	static mat4f mat4f_from_bullet(const btTransform arg) noexcept(true)
	{
		noob::mat4f t;
		arg.getOpenGLMatrix(&t.m[0]);
		return t;
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// VECTOR FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
		static noob::vec3_type<T> negate(const noob::vec3_type<T>& arg) noexcept(true)
		{
			return arg * -1.0;
		}

	template <typename T>
		static bool vec3_equality(const vec3_type<T> first, const vec3_type<T> second) noexcept(true)
		{
			for (size_t i = 0; i < 3; ++i)
			{
				if (!compare_floats(first.v[i], second.v[i])) return false;
			}
			return true;
		}

	template <typename T>
		static float length_squared(const vec3_type<T> v) noexcept(true)
		{
			return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
		}

	template <typename T>
		static float length(const vec3_type<T> v) noexcept(true)
		{
			return sqrt(v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
		}

	template <typename T>
		static vec3_type<T> normalize(const vec3_type<T> v) noexcept(true)
		{
			vec3_type<T> vb;
			float len = length(v);
			if (0.0 == len)
			{
				return vec3_type<T>(0.0, 0.0, 0.0);
			}
			vb.v[0] = v.v[0] / len;
			vb.v[1] = v.v[1] / len;
			vb.v[2] = v.v[2] / len;
			return vb;
		}

	template <typename T>
		static float dot(const vec3_type<T>& a, const vec3_type<T>& b) noexcept(true)
		{
			return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
		}

	template <typename T>
		static vec3_type<T> cross(const vec3_type<T>& a, const vec3_type<T>& b) noexcept(true)
		{
			float x = a.v[1] * b.v[2] - a.v[2] * b.v[1];
			float y = a.v[2] * b.v[0] - a.v[0] * b.v[2];
			float z = a.v[0] * b.v[1] - a.v[1] * b.v[0];
			return vec3_type<T>(x, y, z);
		}

	template <typename T>
		static float get_squared_dist(const vec3_type<T> from, const vec3_type<T> to) noexcept(true)
		{
			float x = (to.v[0] - from.v[0]) * (to.v[0] - from.v[0]);
			float y = (to.v[1] - from.v[1]) * (to.v[1] - from.v[1]);
			float z = (to.v[2] - from.v[2]) * (to.v[2] - from.v[2]);
			return x + y + z;
		}

	// Converts an un-normalized direction into a heading in degrees.
	template <typename T>	
		static float direction_to_heading(const vec3_type<T> d) noexcept(true)
		{
			return atan2(-d.v[0], d.v[2]) * NOOB_ONE_RAD_IN_DEG;
		}

	template <typename T>
		static vec3_type<T> heading_to_direction(float degrees) noexcept(true)
		{
			float rad = degrees * NOOB_ONE_DEG_IN_RAD;
			return vec3_type<T>(-sinf(rad), 0.0f, -cosf(rad));
		}

	template <typename T>
		static bool linearly_dependent(const noob::vec3_type<T>& a, const noob::vec3_type<T>& b, const noob::vec3_type<T>& c) noexcept(true)
		{
			// if (a cross b) dot c = 0
			if (dot(cross(a, b), c) == 0.0) return true;
			else return false;
		}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// QUATERNION FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
		static versor_type<T> normalize(const versor_type<T>& q) noexcept(true)
		{
			// norm(q) = q / magnitude (q)
			// magnitude (q) = sqrt (w*w + x*x...)
			// only compute sqrt if interior sum != 1.0
			versor_type<T> qq(q);
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

	template <typename T>
		static float dot(const versor_type<T>& q, const versor_type<T>& r) noexcept(true)
		{
			return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
		}

	template <typename T>
		static versor_type<T> slerp(const versor_type<T>& q, const versor_type<T>& r, float t) noexcept(true)
		{
			versor_type<T> temp_q(q);
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
			// If theta = 180 degrees then result is not fully defined we could rotate around any axis normal to qa or qb
			versor_type<T> result;
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

	template <typename T>
		static versor_type<T> versor_from_mat4(const mat4_type<T>& m) noexcept(true)
		{
			// Eigen::Map<const Eigen::Matrix4f> mat_map(&m.m[0]);
			// const Eigen::Quaternion<float> vv(mat_map); //Eigen::Matrix4f(&m.m[0]));
			// return versor_from_eigen(vv);
			glm::mat4 mm = glm::make_mat4(&m.m[0]);
			glm::quat q = glm::quat_cast(mm);
			noob::versor_type<T> qq;
			qq.q[0] = q[0];
			qq.q[1] = q[1];
			qq.q[2] = q[2];
			qq.q[3] = q[3];
			return qq;
		}

	template <typename T>
		static vec3_type<T> lerp(const noob::vec3_type<T>& a, const noob::vec3_type<T>& b, float t) noexcept(true)
		{ 
			return a + (b - a) * t;
		}

	template <typename T>
		static versor_type<T> versor_from_axis_rad(float radians, float x, float y, float z) noexcept(true)
		{
			versor_type<T> result;
			result.q[0] = cos (radians / 2.0);
			result.q[1] = sin (radians / 2.0) * x;
			result.q[2] = sin (radians / 2.0) * y;
			result.q[3] = sin (radians / 2.0) * z;
			return result;
		}
	template <typename T>
		static versor_type<T> versor_from_axis_deg(float degrees, float x, float y, float z) noexcept(true)
		{
			return versor_from_axis_rad<T>(NOOB_ONE_DEG_IN_RAD * degrees, x, y, z);
		}

	template <typename T>
		static mat4_type<T> versor_to_mat4(const noob::versor_type<T>& q) noexcept(true)
		{
			const float w = q.q[0];
			const float x = q.q[1];
			const float y = q.q[2];
			const float z = q.q[3];
			return mat4_type<T>(1.0f - 2.0f * y * y - 2.0f * z * z,
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


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// MATRIX FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
		static mat3_type<T> zero_mat3() noexcept(true)
		{
			return mat3_type<T>(	0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f);
		}
	template <typename T>
		static mat3_type<T> identity_mat3() noexcept(true)
		{
			return mat3_type<T>(	1.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f);
		}

	template <typename T>
		static mat4_type<T> zero_mat4() noexcept(true)
		{
			return mat4_type<T>(	0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f);
		}

	template <typename T>
		static mat4_type<T> identity_mat4() noexcept(true)
		{
			return mat4_type<T>(	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
		}

	// Returns a scalar value with the determinant for a 4x4 matrix
	// see http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
	template <typename T>	
		static float determinant(const mat4_type<T>& mm) noexcept(true)
		{
			return
				mm.m[12] * mm.m[9] * mm.m[6] * mm.m[3] -
				mm.m[8] * mm.m[13] * mm.m[6] * mm.m[3] -
				mm.m[12] * mm.m[5] * mm.m[10] * mm.m[3] +
				mm.m[4] * mm.m[13] * mm.m[10] * mm.m[3] +
				mm.m[8] * mm.m[5] * mm.m[14] * mm.m[3] -
				mm.m[4] * mm.m[9] * mm.m[14] * mm.m[3] -
				mm.m[12] * mm.m[9] * mm.m[2] * mm.m[7] +
				mm.m[8] * mm.m[13] * mm.m[2] * mm.m[7] +
				mm.m[12] * mm.m[1] * mm.m[10] * mm.m[7] -
				mm.m[0] * mm.m[13] * mm.m[10] * mm.m[7] -
				mm.m[8] * mm.m[1] * mm.m[14] * mm.m[7] +
				mm.m[0] * mm.m[9] * mm.m[14] * mm.m[7] +
				mm.m[12] * mm.m[5] * mm.m[2] * mm.m[11] -
				mm.m[4] * mm.m[13] * mm.m[2] * mm.m[11] -
				mm.m[12] * mm.m[1] * mm.m[6] * mm.m[11] +
				mm.m[0] * mm.m[13] * mm.m[6] * mm.m[11] +
				mm.m[4] * mm.m[1] * mm.m[14] * mm.m[11] -
				mm.m[0] * mm.m[5] * mm.m[14] * mm.m[11] -
				mm.m[8] * mm.m[5] * mm.m[2] * mm.m[15] +
				mm.m[4] * mm.m[9] * mm.m[2] * mm.m[15] +
				mm.m[8] * mm.m[1] * mm.m[6] * mm.m[15] -
				mm.m[0] * mm.m[9] * mm.m[6] * mm.m[15] -
				mm.m[4] * mm.m[1] * mm.m[10] * mm.m[15] +
				mm.m[0] * mm.m[5] * mm.m[10] * mm.m[15];
		}

	// returns a 16-element array that is the inverse of a 16-element array (4x4 matrix). see http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
	template <typename T>
		static mat4_type<T> inverse(const mat4_type<T>& mm) noexcept(true)
		{
			float det = determinant(mm);

			// There is no inverse if determinant is zero (not likely unless scale is broken)
			if (compare_floats(0.0, det))
			{
				// logger::log("WARNING. matrix has no determinant. can not invert");
				return mm;
			}

			float inv_det = 1.0f / det;

			return mat4_type<T> (
					inv_det * (
						mm.m[9] * mm.m[14] * mm.m[7] - mm.m[13] * mm.m[10] * mm.m[7] +
						mm.m[13] * mm.m[6] * mm.m[11] - mm.m[5] * mm.m[14] * mm.m[11] -
						mm.m[9] * mm.m[6] * mm.m[15] + mm.m[5] * mm.m[10] * mm.m[15]
						),
					inv_det * (
						mm.m[13] * mm.m[10] * mm.m[3] - mm.m[9] * mm.m[14] * mm.m[3] -
						mm.m[13] * mm.m[2] * mm.m[11] + mm.m[1] * mm.m[14] * mm.m[11] +
						mm.m[9] * mm.m[2] * mm.m[15] - mm.m[1] * mm.m[10] * mm.m[15]
						),
					inv_det * (
						mm.m[5] * mm.m[14] * mm.m[3] - mm.m[13] * mm.m[6] * mm.m[3] +
						mm.m[13] * mm.m[2] * mm.m[7] - mm.m[1] * mm.m[14] * mm.m[7] -
						mm.m[5] * mm.m[2] * mm.m[15] + mm.m[1] * mm.m[6] * mm.m[15]
						),
					inv_det * (
						mm.m[9] * mm.m[6] * mm.m[3] - mm.m[5] * mm.m[10] * mm.m[3] -
						mm.m[9] * mm.m[2] * mm.m[7] + mm.m[1] * mm.m[10] * mm.m[7] +
						mm.m[5] * mm.m[2] * mm.m[11] - mm.m[1] * mm.m[6] * mm.m[11]
						),
					inv_det * (
							mm.m[12] * mm.m[10] * mm.m[7] - mm.m[8] * mm.m[14] * mm.m[7] -
							mm.m[12] * mm.m[6] * mm.m[11] + mm.m[4] * mm.m[14] * mm.m[11] +
							mm.m[8] * mm.m[6] * mm.m[15] - mm.m[4] * mm.m[10] * mm.m[15]
						  ),
					inv_det * (
							mm.m[8] * mm.m[14] * mm.m[3] - mm.m[12] * mm.m[10] * mm.m[3] +
							mm.m[12] * mm.m[2] * mm.m[11] - mm.m[0] * mm.m[14] * mm.m[11] -
							mm.m[8] * mm.m[2] * mm.m[15] + mm.m[0] * mm.m[10] * mm.m[15]
						  ),
					inv_det * (
							mm.m[12] * mm.m[6] * mm.m[3] - mm.m[4] * mm.m[14] * mm.m[3] -
							mm.m[12] * mm.m[2] * mm.m[7] + mm.m[0] * mm.m[14] * mm.m[7] +
							mm.m[4] * mm.m[2] * mm.m[15] - mm.m[0] * mm.m[6] * mm.m[15]
						  ),
					inv_det * (
							mm.m[4] * mm.m[10] * mm.m[3] - mm.m[8] * mm.m[6] * mm.m[3] +
							mm.m[8] * mm.m[2] * mm.m[7] - mm.m[0] * mm.m[10] * mm.m[7] -
							mm.m[4] * mm.m[2] * mm.m[11] + mm.m[0] * mm.m[6] * mm.m[11]
						  ),
					inv_det * (
							mm.m[8] * mm.m[13] * mm.m[7] - mm.m[12] * mm.m[9] * mm.m[7] +
							mm.m[12] * mm.m[5] * mm.m[11] - mm.m[4] * mm.m[13] * mm.m[11] -
							mm.m[8] * mm.m[5] * mm.m[15] + mm.m[4] * mm.m[9] * mm.m[15]
						  ),
					inv_det * (
							mm.m[12] * mm.m[9] * mm.m[3] - mm.m[8] * mm.m[13] * mm.m[3] -
							mm.m[12] * mm.m[1] * mm.m[11] + mm.m[0] * mm.m[13] * mm.m[11] +
							mm.m[8] * mm.m[1] * mm.m[15] - mm.m[0] * mm.m[9] * mm.m[15]
						  ),
					inv_det * (
							mm.m[4] * mm.m[13] * mm.m[3] - mm.m[12] * mm.m[5] * mm.m[3] +
							mm.m[12] * mm.m[1] * mm.m[7] - mm.m[0] * mm.m[13] * mm.m[7] -
							mm.m[4] * mm.m[1] * mm.m[15] + mm.m[0] * mm.m[5] * mm.m[15]
						  ),
					inv_det * (
							mm.m[8] * mm.m[5] * mm.m[3] - mm.m[4] * mm.m[9] * mm.m[3] -
							mm.m[8] * mm.m[1] * mm.m[7] + mm.m[0] * mm.m[9] * mm.m[7] +
							mm.m[4] * mm.m[1] * mm.m[11] - mm.m[0] * mm.m[5] * mm.m[11]
						  ),
					inv_det * (
							mm.m[12] * mm.m[9] * mm.m[6] - mm.m[8] * mm.m[13] * mm.m[6] -
							mm.m[12] * mm.m[5] * mm.m[10] + mm.m[4] * mm.m[13] * mm.m[10] +
							mm.m[8] * mm.m[5] * mm.m[14] - mm.m[4] * mm.m[9] * mm.m[14]
						  ),
					inv_det * (
							mm.m[8] * mm.m[13] * mm.m[2] - mm.m[12] * mm.m[9] * mm.m[2] +
							mm.m[12] * mm.m[1] * mm.m[10] - mm.m[0] * mm.m[13] * mm.m[10] -
							mm.m[8] * mm.m[1] * mm.m[14] + mm.m[0] * mm.m[9] * mm.m[14]
						  ),
					inv_det * (
							mm.m[12] * mm.m[5] * mm.m[2] - mm.m[4] * mm.m[13] * mm.m[2] -
							mm.m[12] * mm.m[1] * mm.m[6] + mm.m[0] * mm.m[13] * mm.m[6] +
							mm.m[4] * mm.m[1] * mm.m[14] - mm.m[0] * mm.m[5] * mm.m[14]
						  ),
					inv_det * (
							mm.m[4] * mm.m[9] * mm.m[2] - mm.m[8] * mm.m[5] * mm.m[2] +
							mm.m[8] * mm.m[1] * mm.m[6] - mm.m[0] * mm.m[9] * mm.m[6] -
							mm.m[4] * mm.m[1] * mm.m[10] + mm.m[0] * mm.m[5] * mm.m[10]
						  )
						);
		}

	// Returns a 16-element array flipped on the main diagonal
	template <typename T>
		static mat4_type<T> transpose(const mat4_type<T>& mm) noexcept(true)
		{
			return mat4_type<T>(
					mm.m[0], mm.m[4], mm.m[8], mm.m[12],
					mm.m[1], mm.m[5], mm.m[9], mm.m[13],
					mm.m[2], mm.m[6], mm.m[10], mm.m[14],
					mm.m[3], mm.m[7], mm.m[11], mm.m[15]
					);
		}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AFFINE MATRIX FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	template <typename T>
		static mat4_type<T> translate(const mat4_type<T>& m, const vec3_type<T> v) noexcept(true)
		{
			mat4_type<T> m_t = identity_mat4<T>();
			m_t.m[12] = v.v[0];
			m_t.m[13] = v.v[1];
			m_t.m[14] = v.v[2];
			return m_t * m;
		}

	template <typename T>
		static mat4_type<T> rotate(const mat4_type<T>& m, const versor_type<T>& v) noexcept(true)
		{
			return versor_to_mat4(v) * m;
		}

	template <typename T>
		static mat4_type<T> rotate_x_deg(const mat4_type<T>& m, float deg) noexcept(true)
		{
			// convert to radians
			float rad = deg * NOOB_ONE_DEG_IN_RAD;
			mat4_type<T> m_r = identity_mat4<T>();
			m_r.m[5] = cos(rad);
			m_r.m[9] = -sin(rad);
			m_r.m[6] = sin(rad);
			m_r.m[10] = cos(rad);
			return m_r * m;
		}

	template <typename T>
		static mat4_type<T> rotate_y_deg(const mat4_type<T>& m, float deg) noexcept(true)
		{
			// convert to radians
			const float rad = deg * NOOB_ONE_DEG_IN_RAD;
			mat4_type<T> m_r = identity_mat4<T>();
			m_r.m[0] = cos(rad);
			m_r.m[8] = sin(rad);
			m_r.m[2] = -sin(rad);
			m_r.m[10] = cos(rad);
			return m_r * m;
		}

	template <typename T>
		static mat4_type<T> rotate_z_deg(const mat4_type<T>& m, float deg) noexcept(true)
		{
			// convert to radians
			const float rad = deg * NOOB_ONE_DEG_IN_RAD;
			mat4_type<T> m_r = identity_mat4<T>();
			m_r.m[0] = cos(rad);
			m_r.m[4] = -sin(rad);
			m_r.m[1] = sin(rad);
			m_r.m[5] = cos(rad);
			return m_r * m;
		}

	template <typename T>
		static mat4_type<T> scale(const mat4_type<T>& m, const vec3_type<T> v) noexcept(true)
		{
			mat4_type<T> a = identity_mat4<T>();
			a.m[0] = v.v[0];
			a.m[5] = v.v[1];
			a.m[10] = v.v[2];
			return a * m;
		}
	template <typename T>
		static vec3_type<T> get_normal(const std::array<vec3_type<T>, 3>& vertices) noexcept(true)
		{
			// Note: We onlt need the first three points on the face to calculate its normal.
			noob::vec3_type<T> u = vertices[1] - vertices[0];
			noob::vec3_type<T> v = vertices[2] - vertices[0];
			noob::vec3_type<T> results = noob::cross(u, v);
			return normalize(results);
		}

	// TODO: Implement. PITA (?)
	// vec4 rotation_from_mat4(const mat4& m);
	template <typename T>
		static vec3_type<T> translation_from_mat4(const mat4_type<T>& m) noexcept(true)
		{
			vec3_type<T> v;
			v.v[0] = m.m[12];
			v.v[1] = m.m[13];
			v.v[2] = m.m[14];
			return v;
		}

	template <typename T>
		static vec3_type<T> scale_from_mat4(const mat4_type<T>& m) noexcept(true)
		{
			vec3_type<T> v;
			v.v[0] = m.m[0];
			v.v[1] = m.m[5];
			v.v[2] = m.m[10];
			return v;
		}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CAMERA FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Returns a view matrix using the opengl lookAt style. COLUMN ORDER:
	template <typename T>
		static mat4_type<T> look_at(const vec3_type<T> cam_pos, const vec3_type<T> targ_pos, const vec3_type<T> up) noexcept(true)
		{
			// inverse translation
			mat4_type<T> p = identity_mat4<T>();
			p = translate(p, vec3_type<T>(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
			// distance vector
			vec3_type<T> d = targ_pos - cam_pos;
			// forward vector
			vec3_type<T> f = normalize(d);
			// right vector
			vec3_type<T> r = normalize(cross(f, up));
			// real up vector
			vec3_type<T> u = normalize(cross(r, f));
			mat4_type<T> ori = identity_mat4<T>();
			ori.m[0] = r.v[0];
			ori.m[4] = r.v[1];
			ori.m[8] = r.v[2];
			ori.m[1] = u.v[0];
			ori.m[5] = u.v[1];
			ori.m[9] = u.v[2];
			ori.m[2] = -f.v[0];
			ori.m[6] = -f.v[1];
			ori.m[10] = -f.v[2];

			return ori * p;//p * ori;
		}

	// returns a perspective function mimicking the opengl projection style.
	template <typename T>
		static mat4_type<T> perspective(float fovy, float aspect, float near, float far) noexcept(true)
		{
			float fov_rad = fovy * NOOB_ONE_DEG_IN_RAD;
			float range = tan (fov_rad / 2.0f) * near;
			float sx = (2.0f * near) / (range * aspect + range * aspect);
			float sy = near / range;
			float sz = -(far + near) / (far - near);
			float pz = -(2.0f * far * near) / (far - near);
			mat4_type<T> m = zero_mat4<T>(); // make sure bottom-right corner is zero
			m.m[0] = sx;
			m.m[5] = sy;
			m.m[10] = sz;
			m.m[14] = pz;
			m.m[11] = -1.0f;
			return m;
		}

	/*
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15
	   */
	template <typename T>
		static mat4_type<T> ortho(float left, float right, float bottom, float top, float near, float far) noexcept(true)
		{
			mat4_type<T> m = zero_mat4<T>();
			m.m[0] = 2.0/(right-left);
			m.m[5] = 2.0/(top - bottom);
			m.m[10] = -2.0/(far - near);
			m.m[12] = (right + left)/(right - left);
			m.m[13] = (top + bottom)/(top - bottom);
			m.m[14] = (far + near)/(far - near);
			m.m[15] = 1.0;
			return m;
		}


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GEOMETRY UTILITY FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename T>
		static bbox_type<T> update_bbox_type(const noob::bbox_type<T>& b, const noob::vec3_type<T>& v)
		{
			bbox_type<T> results;

			results.min[0] = std::min(b.min[0], v[0]);
			results.min[1] = std::min(b.min[1], v[1]);
			results.min[2] = std::min(b.min[2], v[2]);

			results.max[0] = std::max(b.max[0], v[0]);
			results.max[1] = std::max(b.max[1], v[1]);
			results.max[2] = std::max(b.max[2], v[2]);

			return results;
		}

	template <typename T>
		static bbox_type<T> update_bbox_type(const noob::bbox_type<T>& a, const noob::bbox_type<T>& b)
		{
			noob::bbox_type<T> results;

			results = update_bbox_type(a, b.min);
			results = update_bbox_type(results, b.max);

			return results;
		}
}
