#pragma once

#include <array>

#include <glm/glm.hpp>
#include "MathDefines.hpp"

namespace noob
{
	struct vec4;
	struct versor;
	/* stored like this:
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15 */
	struct mat4
	{
		mat4() {}

		mat4(float a, float b, float c, float d, float e, float f, float g, float h, float i, float j, float k, float l, float mm, float n, float o, float p)
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
			m[9] = j;
			m[10] = k;
			m[11] = l;
			m[12] = mm;
			m[13] = n;
			m[14] = o;
			m[15] = p;
		}

		mat4(const std::array<float,16>& mm)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = mm[i];
			}
		}

		mat4(const glm::mat4& mm)
		{
			const float *source = (const float*)glm::value_ptr(mm);
			for (size_t i = 0; i < 16; ++i)
			{
				m[i] = source[i];
			}
		}

		mat4(const btTransform& xform)
		{
			xform.getOpenGLMatrix(&m[0]);
		}

		float& operator[](int x) 
		{
			return m[x];
		}

		vec4 mat4::operator*(const vec4& rhs) const
		{
			// 0x + 4y + 8z + 12w
			float x = m[0] * rhs.v[0] +
				m[4] * rhs.v[1] +
				m[8] * rhs.v[2] +
				m[12] * rhs.v[3];
			// 1x + 5y + 9z + 13w
			float y = m[1] * rhs.v[0] +
				m[5] * rhs.v[1] +
				m[9] * rhs.v[2] +
				m[13] * rhs.v[3];
			// 2x + 6y + 10z + 14w
			float z = m[2] * rhs.v[0] +
				m[6] * rhs.v[1] +
				m[10] * rhs.v[2] +
				m[14] * rhs.v[3];
			// 3x + 7y + 11z + 15w
			float w = m[3] * rhs.v[0] +
				m[7] * rhs.v[1] +
				m[11] * rhs.v[2] +
				m[15] * rhs.v[3];
			return vec4 (x, y, z, w);
		}

		mat4 mat4::operator*(const mat4& rhs) const
		{
			mat4 r = mat4::zero();
			int r_index = 0;
			for (int col = 0; col < 4; col++)
			{
				for (int row = 0; row < 4; row++) 
				{
					float sum = 0.0f;
					for (int i = 0; i < 4; i++) 
					{
						sum += rhs.m[i + col * 4] * m[row + i * 4];
					}
					r.m[r_index] = sum;
					r_index++;
				}
			}
			return r;
		}

		mat4& mat4::operator=(const mat4& rhs)
		{
			for (int i = 0; i < 16; i++)
			{
				m[i] = rhs.m[i];
			}
			return *this;
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



		static mat4 zero()
		{
			return mat4(	0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 0.0f);
		}

		static mat4 identity()
		{
			return mat4(	1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f);
		}


		// Returns a scalar value with the determinant for a 4x4 matrix
		// see http://www.euclideanspace.com/maths/algebra/matrix/functions/determinant/fourD/index.htm
		static float determinant(const mat4& mm)
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
		static mat4 inverse(const mat4& mm)
		{
			float det = determinant (mm);
			/* there is no inverse if determinant is zero (not likely unless scale is
			   broken) */
			if (0.0f == det)
			{
				fprintf (stderr, "WARNING. matrix has no determinant. can not invert\n");
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
		static mat4 transpose(const mat4& mm)
		{
			return mat4 (
					mm.m[0], mm.m[4], mm.m[8], mm.m[12],
					mm.m[1], mm.m[5], mm.m[9], mm.m[13],
					mm.m[2], mm.m[6], mm.m[10], mm.m[14],
					mm.m[3], mm.m[7], mm.m[11], mm.m[15]
				    );
		}



		std::string to_string() const
		{
			fmt::MemoryWriter w;
			w << m[0] << ", " << m[4] << ", " << m[8] << ", " << m[12] << "\n" << m[1] << ", " << m[5] << ", " << m[9] << ", " << m[13] << "\n" << m[2] << ", " << m[6] << ", " << m[10] << ", " << m[14] << "\n" << m[3] << ", " << m[7] << ", " << m[11] << ", " << m[15] << "\n";
			return w.str();
		}

		std::array<float, 16> m;
	};
}
