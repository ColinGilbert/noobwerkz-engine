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

		static mat3 zero()
		{
			return mat3(	0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f);
		}

		static mat3 mat3::identity()
		{
			return mat3(	1.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 1.0f);
		}


		std::array<float,9> m;
	};
}
