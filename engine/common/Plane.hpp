#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class plane
	{
		public:
			through(noob::vec3& a, const noob::vec3& b, const noob::vec3& c)
			{
				normal = noob::normalize(noob::cross(b - a, c - a));
				w = noob::dot(normal, a);
			}

			bool ok() const
			{
				return noob::length(normal) > 0.0f; 
			}

			void flip()
			{
				normal = noob::negate(normal);
				w *= -1.0;
			}

			float signed_distance(const noob::vec3& p)
			{
				noob::dot(normal, p - w);
			}

		protected:
			noob::vec3 normal;
			float w;
	};
}
