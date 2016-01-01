// This class collects the individual components of position and then provides the 4x4 affine matrix upon request
#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class transform_helper
	{
		public:
			transform_helper() : rotation(0.0, 0.0, 0.0, 1.0), translation(0.0, 0.0, 0.0), scaling(1.0, 1.0, 1.0), rotated(false), translated(false), scaled(false) {}

			//TODO: Test for correctness.
			noob::mat4 get_matrix() const
			{
				noob::mat4 m = noob::identity_mat4();
				
				if (rotated)
				{
					m = noob::quat_to_mat4(rotation) * m;
				}
				if (scaled)
				{
					m = noob::scale(m, scaling);
				
				}
				if (translated)
				{
					m = noob::translate(m, translation);
				}

				return m;
			}

			void scale(const noob::vec3& s)
			{
				scaled = true;
				scaling.v[0] *= s.v[0];
				scaling.v[1] *= s.v[1];
				scaling.v[2] *= s.v[2];
			}

			void translate(const noob::vec3& t)
			{
				translated = true;
				translation += t;
			}

			void rotate(const noob::versor& quat)
			{
				rotated = true;
				rotation = rotation * quat;
				rotation = noob::normalize(rotation);
			}

			void reset()
			{
				rotated = translated = scaled = false;
				rotation = noob::versor(0.0, 0.0, 0.0, 1.0);
				translation = noob::vec3(0.0, 0.0, 0.0);
				scaling = noob::vec3(1.0, 1.0, 1.0);
			}

		protected:		

			noob::versor rotation;
			noob::vec3 translation;
			noob::vec3 scaling;

			bool rotated, translated, scaled;
	};
}
