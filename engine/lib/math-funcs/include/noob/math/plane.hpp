#pragma once

#include <Eigen/Geometry>
#include "vec3.hpp"

namespace noob
{
	template <typename T>
	class plane
	{
		public:
			void through(noob::vec3_type<T>& a, const noob::vec3_type<T>& b, const noob::vec3_type<T>& c)
			{
				inner = Eigen::Hyperplane<float, 3>::Through(Eigen::Vector3f(b.v[0], b.v[1], b.v[2]), Eigen::Vector3f(c.v[0], c.v[1], c.v[2]));
			}

			void normalize()
			{
				inner.normalize();
			}

			float signed_distance(const noob::vec3_type<T>& p) const
			{
				return inner.signedDistance(Eigen::Vector3f(p.v[0], p.v[1], p.v[2]));
			}

			noob::vec3_type<T> normal() const
			{
				return vec3_from_eigen_vec3(inner.normal());
			}

			float offset() const
			{
				return inner.offset();
			}

			noob::vec3_type<T> projection(const noob::vec3_type<T>& p) const
			{
				return vec3_from_eigen_vec3(inner.projection(Eigen::Vector3f(p.v[0], p.v[1], p.v[2])));
			}


		protected:
			Eigen::Hyperplane<T, 3> inner;
	};
}
