#pragma once
#include <cmath>

#include <cereal/access.hpp>
#include <cereal/types/array.hpp>
#include <cereal/archives/portable_binary.hpp>
#include <cereal/archives/binary.hpp>

#include <array>
#include <assimp/types.h>
#include <btBulletDynamicsCommon.h>
#include <glm/glm.hpp>
#include "format.h"

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyMesh;
typedef OpenMesh::TriMesh_ArrayKernelT<> TriMesh;

// const used to convert degrees into radians
#define TAU 2.0 * M_PI
#define ONE_DEG_IN_RAD (2.0 * M_PI) / 360.0 // 0.017444444
#define ONE_RAD_IN_DEG 360.0 / (2.0 * M_PI) //57.2957795

#define TWO_PI (2.0 * M_PI)

#define NOOB_EPSILON 0.0001

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


	template <typename T> int sign(T val)
	{
		return (T(0) < val) - (val < T(0));
	};

	struct vec2;
	struct vec3;
	struct vec4;
	struct versor;


	struct vec2
	{
		vec2();
		vec2(float x, float y);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(v);
			}


		std::array<float,2> v;

		float& operator[](int x) 
		{
			return v[x];
		}


		float get_opIndex(int i) const

		{
			if (i > 1 ) return v[1];
			if (i < 0) return v[0];
			return v[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 2 && i < 0) return;
			v[i] = value;
		}


		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << v[0] << ", " << v[1] << ")";
			return w.str();
		}
	};

	struct vec3
	{
		vec3();
		// create from 3 scalars
		vec3(float x, float y, float z);
		// create from vec2 and a scalar
		vec3(const vec2& vv, float z);
		// create from truncated vec4
		vec3(const vec4& vv);
		vec3(const vec3& vv);
		vec3(const btVector3&);
		vec3(const PolyMesh::Point&);
		vec3(const std::array<float, 3>&);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(v);
			}

		// add vector to vector
		vec3 operator+(const vec3& rhs) const;
		// add scalar to vector
		vec3 operator+(float rhs) const;
		// because user's expect this too
		vec3& operator+=(const vec3& rhs);
		// subtract vector from vector
		vec3 operator-(const vec3& rhs) const;
		// add vector to vector
		vec3 operator-(float rhs) const;
		// because users expect this too
		vec3& operator-=(const vec3& rhs);
		// multiply with scalar
		vec3 operator*(float rhs) const;
		// because users expect this too
		vec3& operator*=(float rhs);
		// divide vector by scalar
		vec3 operator/(float rhs) const;
		// because users expect this too
		vec3& operator=(const vec3& rhs);

		bool operator==(const vec3& rhs) const;
		bool operator!=(const vec3& rhs) const;

		// internal data
		std::array<float,3> v;

		float& operator[](int x)
		{
			return v[x];
		}

		float get_opIndex(int i) const

		{
			if (i > 2 ) return v[2];
			if (i < 0) return v[0];
			return v[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 2 && i < 0) return;
			v[i] = value;
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
			return w.str();
		}
	};

	struct vec4
	{
		vec4();
		vec4(float x, float y, float z, float w);
		vec4(const vec2& vv, float z, float w);
		vec4(const vec3& vv, float w);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(v);
			}


		std::array<float,4> v;

		float& operator[](int x)
		{
			return v[x];
		}

		float get_opIndex(int i) const

		{
			if (i > 3 ) return v[3];
			if (i < 0) return v[0];
			return v[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 3 && i < 0) return;
			v[i] = value;
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
			return w.str();
		}

	};

	/* stored like this:
	   0 3 6
	   1 4 7
	   2 5 8  */
	struct mat3
	{
		mat3();
		// note! this is entering components in ROW-major order
		mat3(float a, float b, float c,	float d, float e, float f, float g, float h, float i);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(m);
			}

		std::array<float,9> m;

		float& operator[](int x) 
		{
			return m[x];
		}

		float get_opIndex(int i) const

		{
			if (i > 8 ) return m[8];
			if (i < 0) return m[0];
			return m[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 8 && i < 0) return;
			m[i] = value;
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
		mat4(const aiMatrix4x4&);
		mat4(const glm::mat4&);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(m);
			}

		vec4 operator*(const vec4& rhs) const;
		mat4 operator*(const mat4& rhs) const;
		mat4& operator=(const mat4& rhs);
		std::array<float, 16> m;

		float& operator[](int x) 
		{
			return m[x];
		}

		float get_opIndex(int i) const

		{
			if (i > 15) return m[15];
			if (i < 0) return m[0];
			return m[i];
		}

		void set_opIndex(int i, float value)
		{
			if (i > 15 && i < 0) return;
			m[i] = value;
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << m[0] << ", " << m[4] << ", " << m[8] << ", " << m[12] << "\n" << m[1] << ", " << m[5] << ", " << m[9] << ", " << m[13] << "\n" << m[2] << ", " << m[6] << ", " << m[10] << ", " << m[14] << "\n" << m[3] << ", " << m[7] << ", " << m[11] << ", " << m[15] << "\n";
			return w.str();
		}

	};

	struct versor
	{
		versor();
		versor(float,float,float,float);
		versor(const vec4& v);
		versor(const btQuaternion&);

		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(q);
			}


		versor operator/(float rhs) const;
		versor operator*(float rhs) const;
		versor operator*(const versor& rhs) const;
		versor operator+(const versor& rhs) const;
		versor& operator=(const versor& rhs);


		float& operator[](int x) 
		{
			return q[x];
		}

		/*
		   float get_opIndex(int i) const

		   {
		   if (i > 8 ) return m[8];
		   if (i < 0) return m[0];
		   return m[i];
		   }

		   void set_opIndex(int i, float value)
		   {
		   if (i > 8 && i < 0) return;
		   m[i] = value;
		   }
		   */

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

		std::string to_string() const;

		std::array<float,4> q;
	};

	struct bbox
	{
		template <class Archive>
			void serialize( Archive & ar )
			{
				ar(min, max, center);
			}

		noob::vec3 min, max, center;
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

	inline bool compare_floats(float a, float b)
	{
		// http://c-faq.com/fp/fpequal.html
		if (std::fabs(a - b) <= NOOB_EPSILON * std::fabs(a)) return false;
		else return true;
	}

	// vector functions
	float length(const vec3& v);
	inline float length_squared(const vec3& v)
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
	// mat4 rotate(const mat4& m, const vec4& quat);
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
	// versor slerp(const versor& q, const versor& r);
	versor normalize(const versor& q);
	versor slerp(const versor& q, const versor& r, float t);
};

// void create_versor(float* q, float a, float x, float y, float z);
// void mult_quat_quat(float* result, float* r, float* s);
// void mult_quat_quat(versor results, const versor& r, const versor& s);
// void normalize_quat(float* q);
// void quat_to_mat4(float* m, float* q); 
// stupid overloading wouldn't let me use const
