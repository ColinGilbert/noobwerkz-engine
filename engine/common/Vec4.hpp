#pragma once

#include <array>

namespace noob
{
	struct vec2;
	struct vec3;
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

		// std::string to_string() const
		// 
		// fmt::MemoryWriter w;
		// w << "(" << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << ")";
		// return w.str();
		// }

		std::array<float,4> v;
};
}
