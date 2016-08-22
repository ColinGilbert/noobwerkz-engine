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


namespace noob
{
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

	static uint64_t pack_32_to_64(uint32_t x, uint32_t y)
	{
		return static_cast<uint64_t>(x) << 32 | y;
	}

	static std::tuple<uint32_t, uint32_t> pack_64_to_32(uint64_t arg)
	{
		uint32_t x = static_cast<uint32_t>(arg >> 32);
		uint32_t y = static_cast<uint32_t>(x);
		return std::make_tuple(x,y);
	}

	template <typename T> int sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	}

	static bool compare_floats(float a, float b)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a - b) <= NOOB_EPSILON * std::fabs(a)) return false;
		else return true;
	}

	static bool approximately_zero(float a)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a) <= NOOB_EPSILON) return false;
		else return true;
	}
	
	/* stored like this:
	   0 3 6
	   1 4 7
	   2 5 8  */
	struct mat3 final
	{
		mat3();
		// note! this is entering components in ROW-major order
		mat3(float a, float b, float c,	float d, float e, float f, float g, float h, float i);

		std::array<float,9> m;

		float& operator[](int x) 
		{
			return m[x];
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << m[0] << ", " << m[3] << ", " << m[6] << "\n" << m[1] << ", " << m[4] << ", " << m[7] << "\n" << m[2] << ", " << m[5] << ", " << m[8] << "\n";
			return w.str();
		}


	};

	/* stored like this:
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15 */
	struct mat4 
	{
		mat4();
		// note! this is entering components in ROW-major order
		mat4(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float mm, float n, float o, float p);
		mat4(std::array<float,16> mm);
		// mat4(const aiMatrix3x3& AssimpMatrix);
		// mat4(const aiMatrix4x4&);
		mat4(const glm::mat4&);
		mat4(const btTransform&);
		vec4 operator*(const vec4& rhs) const;
		mat4 operator*(const mat4& rhs) const;
		mat4& operator=(const mat4& rhs);
		std::array<float, 16> m;

		float& operator[](uint32_t x) 
		{
			return m[x];
		}
		
		const float& operator[](uint32_t i) const
		{
			return m[i];
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << m[0] << ", " << m[4] << ", " << m[8] << ", " << m[12] << "\n" << m[1] << ", " << m[5] << ", " << m[9] << ", " << m[13] << "\n" << m[2] << ", " << m[6] << ", " << m[10] << ", " << m[14] << "\n" << m[3] << ", " << m[7] << ", " << m[11] << ", " << m[15] << "\n";
			return w.str();
		}

	};

	struct versor final
	{
		versor();
		versor(float,float,float,float);
		versor(const vec4& v);
		versor(const btQuaternion&);

		versor operator/(float rhs) const;
		versor operator*(float rhs) const;
		versor operator*(const versor& rhs) const;
		versor operator+(const versor& rhs) const;
		versor& operator=(const versor& rhs);


		float& operator[](uint32_t x) 
		{
			return q[x];
		}

		const float& operator[](uint32_t x) const
		{
			return q[x];
		}

		float get_opIndex(uint32_t i) const
		{
			if (i > 3 ) return q[3];
			return q[i];
		}

		void set_opIndex(uint32_t i, float value)
		{
			if (i > 3) return;
			q[i] = value;
		}

		std::string to_string() const;

		std::array<float,4> q;
	};


	static vec3 vec3_from_vec4(const vec4& vv)
	{
		noob::vec3 v;
		v.v[0] = vv.v[0];
		v.v[1] = vv.v[1];
		v.v[2] = vv.v[2];
		return v;
	}

	static vec3 vec3_from_array(const std::array<float, 3>& a)
	{
		noob::vec3 v;
		v.v[0] = a[0];
		v.v[1] = a[1];
		v.v[2] = a[2];
		return v;
		// v = a;
	}

	static vec3 vec3_from_bullet(const btVector3& btVec)
	{
		noob::vec3 v;
		v.v[0] = btVec[0];
		v.v[1] = btVec[1];
		v.v[2] = btVec[2];
		return v;
	}

	static vec3 vec3_from_polymesh(const PolyMesh::Point& p)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	static vec3 vec3_from_eigen_vec3(const Eigen::Vector3f& p)
	{
		noob::vec3 v;
		v.v[0] = p[0];
		v.v[1] = p[1];
		v.v[2] = p[2];
		return v;
	}

	// Whatever the hell you gotta do to compile, man...
	static vec3 vec3_from_eigen_block(const Eigen::Block<const Eigen::Matrix<float, 4, 1>, 3, 1, false> n)
	{
		noob::vec3 v;
		v.v[0] = n[0];
		v.v[1] = n[1];
		v.v[2] = n[2];
		return v;
	}

	static noob::vec3 negate(const noob::vec3& arg)
	{
		return arg * -1.0;
	}



	static bool vec3_equality(const vec3& first, const vec3& second)
	{
		for (size_t i = 0; i < 3; ++i)
		{
			if (!compare_floats(first.v[i], second.v[i])) return false;
		}
		return true;
	}

	struct bbox
	{
		//template <class Archive>
		//	void serialize( Archive & ar )
		//	{
		//		ar(min, max, center);
		//	}

		noob::vec3 get_dims() const
		{
			return noob::vec3(std::fabs(min.v[0]) + std::fabs(max.v[0]), std::fabs(min.v[1]) + std::fabs(max.v[1]), std::fabs(max.v[2]) + std::fabs(max.v[2]));
		}

		noob::vec3 min, max;
	};

	struct cubic_region
	{
		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(lower_corner, upper_corner);
			}

		vec3 lower_corner, upper_corner;
	};

	// vector functions
	float length(const vec3& v);
	inline static float length_squared(const vec3& v)
	{
		return v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
	}


	vec3 normalize(const vec3& v);
	float dot(const vec3& a, const vec3& b);
	vec3 cross(const vec3& a, const vec3& b);
	float get_squared_dist(const vec3& from, const vec3& to);
	float direction_to_heading(const vec3& d);
	vec3 heading_to_direction(float degrees);

	inline bool linearly_dependent(const noob::vec3& a, const noob::vec3& b, const noob::vec3& c)
	{
		// if (a cross b) dot c = 0
		if (dot(cross(a, b), c) == 0.0) return true;
		else return false;
	}

	// matrix functions
	mat3 zero_mat3();
	mat3 identity_mat3();
	mat4 zero_mat4();
	mat4 identity_mat4();
	float determinant(const mat4& mm);
	mat4 inverse(const mat4& mm);
	mat4 transpose(const mat4& mm);

	// affine functions
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

	// quaternion functions
	versor quat_from_axis_rad(float radians, float x, float y, float z);
	versor quat_from_axis_deg(float degrees, float x, float y, float z);
	versor quat_from_mat4(const mat4&);
	mat4 quat_to_mat4(const versor& q);
	float dot(const versor& q, const versor& r);
	vec3 lerp(const vec3& a, const vec3& b, float t);
	// versor slerp(const versor& q, const versor& r); 
	versor normalize(const versor& q);
	versor slerp(const versor& q, const versor& r, float t);
}


// void create_versor(float* q, float a, float x, float y, float z);
// void mult_quat_quat(float* result, float* r, float* s);
// void mult_quat_quat(versor results, const versor& r, const versor& s);
// void normalize_quat(float* q);
// void quat_to_mat4(float* m, float* q); 
// stupid overloading wouldn't let me use const
