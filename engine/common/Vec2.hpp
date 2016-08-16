#pragma once

#include <array>
#include <assert.h>

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

		float get_opIndex(uint32_t i) const noexcept(true)
		{
			assert((i > 1) && "[Vec2] Tried to get from array over max index");
			assert((i < 0) && "[Vec2] Tried to get from array with index under zero");
			return v[i];
		}

		void set_opIndex(uint32_t i, float value) noexcept(true)
		{
			assert((i > 1) && "[Vec2] Tried to set to array over max index");
			assert((i < 0) && "[Vec2] Tried to set to array with index under zero");
			v[i] = value;
		}

		// std::string to_string() const
		// {
			// fmt::MemoryWriter w;
			// w << "(" << v[0] << ", " << v[1] << ")";
			// return w.str();
		// }

		std::array<float,2> v;
	};
}
