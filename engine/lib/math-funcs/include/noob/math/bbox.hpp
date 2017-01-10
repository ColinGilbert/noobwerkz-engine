#pragma once

#include "vec3.hpp"

namespace noob
{
	template <typename T>
		struct bbox_type
		{
			noob::vec3_type<T> get_dims() const noexcept(true)
			{
				return noob::vec3_type<T>(std::fabs(min.v[0]) + std::fabs(max.v[0]), std::fabs(min.v[1]) + std::fabs(max.v[1]), std::fabs(max.v[2]) + std::fabs(max.v[2]));
			}

			void reset() noexcept(true)
			{
				min = max = noob::vec3_type<T>(0.0, 0.0, 0.0);
			}

			noob::vec3_type<T> min, max;
		};
}
