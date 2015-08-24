// This class collects the individual components of position and then provides the 4x4 affine matrix upon request
#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class transform_helper
	{
		public:
		transform_helper() : rotation(0.0, 0.0, 0.0, 1.0), translation(0.0, 0.0, 0.0), scaling(1.0, 1.0, 1.0) {}
		
		//TODO: Test for correctness.
		noob::mat4 get_matrix() const
		{
			noob::mat4 m = noob::identity_mat4();
			m = m * noob::quat_to_mat4(rotation);

			m = noob::translate(m, translation);
			m = noob::scale(m, scaling);
			return m;
		}
		
		void scale(const noob::vec3& s)
		{
			scaling += s;
		}

		void translate(const noob::vec3& t)
		{
			translation += t;
		}
		
		void rotate(const noob::versor& quat)
		{
			rotation = rotation * quat;
			rotation = noob::normalize(rotation);
		}
		
		noob::versor rotation;
		noob::vec3 translation;
		noob::vec3 scaling;
	};
}
