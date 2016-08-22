#pragma once

#include <array>

#include "format.h"

namespace noob
{
	struct vec2 final
	{
		vec2() {}

		vec2(float x, float y) noexcept(true)
		{
			v[0] = x;
			v[1] = y;
		}

		float& operator[](uint32_t x) noexcept(true)
		{
			return v[x];
		}

		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << "(" << v[0] << ", " << v[1] << ")";
			return w.str();
		}

		std::array<float,2> v;
	};
}
