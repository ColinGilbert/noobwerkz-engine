// This class is used for C++ convenience, and for providing an API for AngelScript
// TODO: Convert to Eigen and test
#pragma once

#include <cmath>
#include <array>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;

#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include <Eigen/Geometry>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#define NOOB_PI 3.1415926535
#define TAU 2.0 * NOOB_PI
#define ONE_DEG_IN_RAD (2.0 * NOOB_PI) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / (2.0 * NOOB_PI) //57.2957795

#define TWO_PI (2.0 * NOOB_PI)

#define NOOB_EPSILON 0.0001


#include "format.h"

#include "Vec2.hpp"
#include "Vec3.hpp"
#include "Vec3.hpp"
#include "Vec4.hpp"
#include "Versor.hpp"
#include "Mat3.hpp"
#include "Mat4.hpp"
#include "MathUtilTypes.hpp"

namespace noob
{
	// ONE-FLOAT FUNCTIONS:
	static uint64_t pack_32_to_64(uint32_t x, uint32_t y) noexcept(true)
	{
		return static_cast<uint64_t>(x) << 32 | y;
	}

	static std::tuple<uint32_t, uint32_t> pack_64_to_32(uint64_t arg) noexcept(true)
	{
		uint32_t x = static_cast<uint32_t>(arg >> 32);
		uint32_t y = static_cast<uint32_t>(x);
		return std::make_tuple(x,y);
	}

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

	// TWO-FLOAT FUNCTION(S):
	static bool compare_floats(float a, float b) noexcept(true)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a - b) <= NOOB_EPSILON * std::fabs(a)) return false;
		else return true;
	}

	// VECTOR FUNCTIONS:
	static noob::vec3 negate(const noob::vec3& arg) noexcept(true)
	{
		return arg * -1.0;
	}

	static bool vec3_equality(const vec3& first, const vec3& second) noexcept(true)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if (!compare_floats(first.v[i], second.v[i])) return false;
		}
		return true;
	}

	float length(const vec3& v);

	inline static float length_squared(const vec3& v) noexcept(true)
	{
		return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
	}


	vec3 normalize(const vec3& v);
	float dot(const vec3& a, const vec3& b);
	vec3 cross(const vec3& a, const vec3& b);
	float get_squared_dist(const vec3& from, const vec3& to);
	float direction_to_heading(const vec3& d);
	vec3 heading_to_direction(float degrees);

	static bool linearly_dependent(const noob::vec3& a, const noob::vec3& b, const noob::vec3& c) noexcept(true)
	{
		// if (a cross b) dot c = 0
		if (dot(cross(a, b), c) == 0.0) return true;
		else return false;
	}

	// MATRIX FUNCTIONS:
	static mat3 zero_mat3() noexcept(true)
	{
		return mat3(	0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f);
	}

	static mat3 identity_mat3() noexcept(true)
	{
		return mat3(	1.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f);
	}

	static mat4 zero_mat4() noexcept(true)
	{
		return mat4(	0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 0.0f);
	}

	static mat4 identity_mat4() noexcept(true)
	{
		return mat4(	1.0f, 0.0f, 0.0f, 0.0f,
				0.0f, 1.0f, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
	}

	// Returns a scalar value with the determinant for a 4x4 matrix
	// see http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
	static float determinant(const mat4& mm) noexcept(true)
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


	/* returns a 16-element array that is the inverse of a 16-element array (4x4
	   matrix). see http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm */
	static mat4 inverse(const mat4& mm) noexcept(true)
	{
		float det = determinant (mm);
		/* there is no inverse if determinant is zero (not likely unless scale is
		   broken) */
		if (0.0f == det)
		{
			logger::log("WARNING. matrix has no determinant. can not invert");
			return mm;
		}
		float inv_det = 1.0f / det;

		return mat4 (
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
	static mat4 transpose(const mat4& mm) noexcept(true)
	{
		return mat4 (
				mm.m[0], mm.m[4], mm.m[8], mm.m[12],
				mm.m[1], mm.m[5], mm.m[9], mm.m[13],
				mm.m[2], mm.m[6], mm.m[10], mm.m[14],
				mm.m[3], mm.m[7], mm.m[11], mm.m[15]
			    );
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// AFFINE MATRIX FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	mat4 translate(const mat4& m, const vec3& v);
	// TODO: Find out if rotating a matrix by a quaternion can be done by converting the quat to a matrix and then multiplying the two.
	mat4 rotate(const mat4& m, const versor&);
	mat4 rotate_x_deg(const mat4& m, float deg);
	mat4 rotate_y_deg(const mat4& m, float deg);
	mat4 rotate_z_deg(const mat4& m, float deg);
	mat4 scale(const mat4& m, const vec3& v);

	noob::vec3 get_normal(const std::array<noob::vec3, 3>& tri);

	// extraction of useful info from mat4

	// TODO: Implement. PITA (?)
	// vec4 rotation_from_mat4(const mat4& m);
	vec3 translation_from_mat4(const mat4& m);
	vec3 scale_from_mat4(const mat4& m);

	// camera functions
	mat4 look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up);
	mat4 perspective(float fovy, float aspect, float near, float far);
	mat4 ortho(float left, float right, float bottom, float top, float near, float far);



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// QUATERNION FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	// quaternion functions
	versor quat_from_axis_rad(float radians, float x, float y, float z);
	versor quat_from_axis_deg(float degrees, float x, float y, float z);
	versor quat_from_mat4(const mat4&);
	mat4 quat_to_mat4(const versor& q);
	float dot(const versor& q, const versor& r);
	vec3 lerp(const vec3& a, const vec3& b, float t);

	static versor normalize(const versor& q) noexcept(true)
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

	static versor versor_from_axis_rad(float radians, float x, float y, float z) noexcept(true)
	{
		versor result;
		result.q[0] = cos (radians / 2.0);
		result.q[1] = sin (radians / 2.0) * x;
		result.q[2] = sin (radians / 2.0) * y;
		result.q[3] = sin (radians / 2.0) * z;
		return result;
	}

	static versor versor_from_axis_deg(float degrees, float x, float y, float z) noexcept(true)
	{
		return versor_from_axis_rad (ONE_DEG_IN_RAD * degrees, x, y, z);
	}

	static mat4 versor_to_mat4(const noob::versor& q) noexcept(true)
	{
		const float w = q.q[0];
		const float x = q.q[1];
		const float y = q.q[2];
		const float z = q.q[3];
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

	static float dot(const versor& q, const versor& r) noexcept(true)
	{
		return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
	}

	static versor slerp(const versor& q, const versor& r, float t) noexcept(true)
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

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// CONVERSION UTILITY FUNCTIONS:
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static vec3 vec3_from_vec4(const vec4& vv) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = vv.v[0];
		v.v[1] = vv.v[1];
		v.v[2] = vv.v[2];
		return v;
	}

	static vec3 vec3_from_array(const std::array<float, 3>& a) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = a[0];
		v.v[1] = a[1];
		v.v[2] = a[2];
		return v;
		// v = a;
	}

	static vec3 vec3_from_bullet(const btVector3& btVec) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = btVec[0];
		v.v[1] = btVec[1];
		v.v[2] = btVec[2];
		return v;
	}

	static vec3 vec3_from_polymesh(const PolyMesh::Point& p) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	static vec3 vec3_from_eigen_vec3(const Eigen::Vector3f& p) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	// Whatever the hell you gotta do to compile, man...
	static vec3 vec3_from_eigen_block(const Eigen::Block<const Eigen::Matrix<float, 4, 1>, 3, 1, false> n) noexcept(true)
	{
		noob::vec3 v;
		v.v[0] = n[0];
		v.v[1] = n[1];
		v.v[2] = n[2];
		return v;
	}

	static versor versor_from_mat4(const mat4& m) noexcept(true)
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
}


// void create_versor(float* q, float a, float x, float y, float z);
// void mult_quat_quat(float* result, float* r, float* s);
// void mult_quat_quat(versor results, const versor& r, const versor& s);
// void normalize_quat(float* q);
// void quat_to_mat4(float* m, float* q); 
// stupid overloading wouldn't let me use const
