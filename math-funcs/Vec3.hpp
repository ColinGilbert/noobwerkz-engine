#pragma once

#include <array>
#include <assert.h>

#include "Vec2.hpp"
#include "format.h"

namespace noob
{
	struct vec3 final
	{

		std::array<float,3> v;

		vec3() {}

		vec3(float x, float y, float z)
		{
			v[0] = x;
			v[1] = y;
			v[2] = z;
		}

		vec3(const vec2& vv, float z)
		{
			v[0] = vv.v[0];
			v[1] = vv.v[1];
			v[2] = z;
		}

		vec3(const vec3& vv)
		{
			v[0] = vv.v[0];
			v[1] = vv.v[1];
			v[2] = vv.v[2];
		}


		vec3 operator+(const vec3& rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] + rhs.v[0];
			vc.v[1] = v[1] + rhs.v[1];
			vc.v[2] = v[2] + rhs.v[2];
			return vc;
		}

		vec3& operator+=(const vec3& rhs)
		{
			v[0] += rhs.v[0];
			v[1] += rhs.v[1];
			v[2] += rhs.v[2];
			return *this; // return self
		}

		vec3 operator-(const vec3& rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] - rhs.v[0];
			vc.v[1] = v[1] - rhs.v[1];
			vc.v[2] = v[2] - rhs.v[2];
			return vc;
		}

		vec3& operator-=(const vec3& rhs)
		{
			v[0] -= rhs.v[0];
			v[1] -= rhs.v[1];
			v[2] -= rhs.v[2];
			return *this;
		}

		vec3 operator+(float rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] + rhs;
			vc.v[1] = v[1] + rhs;
			vc.v[2] = v[2] + rhs;
			return vc;
		}

		vec3 operator-(float rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] - rhs;
			vc.v[1] = v[1] - rhs;
			vc.v[2] = v[2] - rhs;
			return vc;
		}

		vec3 operator*(float rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] * rhs;
			vc.v[1] = v[1] * rhs;
			vc.v[2] = v[2] * rhs;
			return vc;
		}

		vec3 operator/(float rhs) const
		{
			vec3 vc;
			vc.v[0] = v[0] / rhs;
			vc.v[1] = v[1] / rhs;
			vc.v[2] = v[2] / rhs;
			return vc;
		}

		vec3& operator*=(float rhs) 
		{
			v[0] = v[0] * rhs;
			v[1] = v[1] * rhs;
			v[2] = v[2] * rhs;
			return *this;
		}

		vec3& operator=(const vec3& rhs)
		{
			v[0] = rhs.v[0];
			v[1] = rhs.v[1];
			v[2] = rhs.v[2];
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
			if (i > 2 ) return v[2];
			if (i < 0) return v[0];
			return v[i];
		}

		void set_opIndex(uint32_t i, float value)
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
}
