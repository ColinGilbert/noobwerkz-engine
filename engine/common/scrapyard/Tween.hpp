// Tweens represent an "in-between" state of motion. Linear interpolation for motion, slerp for orientation.
#pragma once

#include <vector>
#include <tuple>

namespace noob
{
	template <typename T>
	class tween
	{
		public:
			void add(float time, T t)
			{
				keyframes.push_back(std::make_tuple(time, t));
			}

			// Sorts keyframes by time.
			void sort()
			{
				std::sort(keyframes.begin(), keyframes.end(), custom_less_than());
			}
			
			// Float argument in std::function is a value between 0.0 and 1.0. Nonsense results otherwise.
			T interpolate(float time, std::function<T(float, T, T)> f)
			{
				auto p = std::find_if(keyframes.begin(), keyframes.end(), [time](const std::tuple<float, T>& t)
				{
        				return std::get<0>(time) > time;
				});
				
				T ret_val = std::get<1>(keyframes.end()->second);

				if (p != keyframes.end())
				{
					if (p == keyframes.end() - 1)
					{
						ret_val = std::get<1>(p->second);
					}
					else
					{
						auto first_tuple = p->second;
						auto second_tuple = (p + 1)->second;
						float interpolated_time = time / (std::get<0>(first_tuple) + std::get<0>(second_tuple);
						ret_val = f(interpolated_time, std::get<1>(first_tuple), std::get<1>(second_tuple));
					}
				}

				return ret_val;
			}


		protected:

			struct custom_less_than
			{
    				bool operator()(std::tuple<float, T> const& lhs, std::tuple<float, T> const& rhs) const
				{
					return std::get<0>(lhs) < std::get<0>(rhs);
				}
			};

			std::vector<float, T>> keyframes;
	};
}
