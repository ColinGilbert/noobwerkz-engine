#pragma once

#include <array>
#include "MathDefines.hpp"

namespace noob
{
	/* stored like this:
	   0 3 6
	   1 4 7
	   2 5 8  */
	struct mat3
	{
		mat3() noexcept(true){}

		/* note: entered in COLUMNS */
		mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i) noexcept(true)
		{
			m[0] = a;
			m[1] = b;
			m[2] = c;
			m[3] = d;
			m[4] = e;
			m[5] = f;
			m[6] = g;
			m[7] = h;
			m[8] = i;
		}

		float& operator[](int x) noexcept(true)
		{
			return m[x];
		}

		std::string to_string() const noexcept(true)
		{	
			fmt::MemoryWriter w;
			w << m[0] << ", " << m[3] << ", " << m[6] << "\n" << m[1] << ", " << m[4] << ", " << m[7] << "\n" << m[2] << ", " << m[5] << ", " << m[8] << "\n";
			return w.str();
		}


		std::array<float,9> m;
	};
}
