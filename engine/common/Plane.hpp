#pragma once

#include "MathFuncs.hpp"
#include <Eigen/Geometry>

namespace noob
{
	class plane
	{
		public:
			void through(noob::vec3& a, const noob::vec3& b, const noob::vec3& c)
			{
				inner = Eigen::Hyperplane<float, 3>::Through(Eigen::Vector3f(b.v[0], b.v[1], b.v[2]), Eigen::Vector3f(c.v[0], c.v[1], c.v[2]));
			}

			void normalize()
			{
				inner.normalize();
			}

			float signed_distance(const noob::vec3& p) const
			{
				return inner.signedDistance(Eigen::Vector3f(p.v[0], p.v[1], p.v[2]));
			}

			noob::vec3 normal() const
			{
				return inner.normal();
			}

			float offset() const
			{
				return inner.offset();
			}

			noob::vec3 projection(const noob::vec3& p) const
			{
				return inner.projection(Eigen::Vector3f(p.v[0], p.v[1], p.v[2]));
			}


		protected:
			Eigen::Hyperplane<float, 3> inner;
	};
}
