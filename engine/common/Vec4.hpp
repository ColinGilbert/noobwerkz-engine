#pragma once

#include <array>
#include "format.h"

#include "Vec2.hpp"
#include "Vec3.hpp"

namespace noob
{
	// struct vec2;
	// struct vec4;

	struct vec4
	{
		vec4() {}

		vec4(float x, float y, float z, float w)
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
			v[3] = w;
		}

		vec4(const vec2& vv, float z, float w) 
		{
			v[0] = vv.v[0];
			v[1] = vv.v[1];
			v[2] = z;
			v[3] = w;
		}

		vec4(const vec3& vv, float w) 
		{
			v[0] = vv.v[0];
			v[1] = vv.v[1];
			v[2] = vv.v[2];
			v[3] = w;
		}


		vec4 operator+(const vec4& rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] + rhs.v[0];
			vc.v[1] = v[1] + rhs.v[1];
			vc.v[2] = v[2] + rhs.v[2];
			vc.v[3] = v[3] + rhs.v[3];
			return vc;
		}

		vec4& operator+=(const vec4& rhs)
		{
			v[0] += rhs.v[0];
			v[1] += rhs.v[1];
			v[2] += rhs.v[2];
			v[3] += rhs.v[3];
			return *this; // return self
		}

		vec4 operator-(const vec4& rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] - rhs.v[0];
			vc.v[1] = v[1] - rhs.v[1];
			vc.v[2] = v[2] - rhs.v[2];
			vc.v[3] = v[3] - rhs.v[3];
			return vc;
		}

		vec4& operator-=(const vec4& rhs)
		{
			v[0] -= rhs.v[0];
			v[1] -= rhs.v[1];
			v[2] -= rhs.v[2];
			v[3] -= rhs.v[3];
			return *this;
		}

		vec4 operator+(float rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] + rhs;
			vc.v[1] = v[1] + rhs;
			vc.v[2] = v[2] + rhs;
			vc.v[3] = v[3] + rhs;
			return vc;
		}

		vec4 operator-(float rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] - rhs;
			vc.v[1] = v[1] - rhs;
			vc.v[2] = v[2] - rhs;
			vc.v[3] = v[3] - rhs;
			return vc;
		}

		vec4 operator*(float rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] * rhs;
			vc.v[1] = v[1] * rhs;
			vc.v[2] = v[2] * rhs;
			vc.v[3] = v[3] * rhs;
			return vc;
		}

		vec4 operator/(float rhs) const
		{
			vec4 vc;
			vc.v[0] = v[0] / rhs;
			vc.v[1] = v[1] / rhs;
			vc.v[2] = v[2] / rhs;
			vc.v[3] = v[3] / rhs;
			return vc;
		}

		vec4& operator*=(float rhs) 
		{
			v[0] = v[0] * rhs;
			v[1] = v[1] * rhs;
			v[2] = v[2] * rhs;
			v[3] = v[3] * rhs;
			return *this;
		}

		vec4& operator=(const vec4& rhs)
		{
			v[0] = rhs.v[0];
			v[1] = rhs.v[1];
			v[2] = rhs.v[2];
			v[3] = rhs.v[3];
			return *this;
		}

		float& operator[](uint32_t x)
		{
			return v[x];
		}

		const float& operator[](uint32_t x) const
		{
			return v[x];
		}


		float get_opIndex(uint32_t i) const

		{
			if (i > 3 ) return v[3];
			return v[i];
		}

		void set_opIndex(uint32_t i, float value)
		{
			if (i > 3) return;
			v[i] = value;
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
			return w.str();
		}

		std::array<float,4> v;
	};

}
