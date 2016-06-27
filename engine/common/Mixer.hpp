#pragma once

#include <Eigen/Eigen.h>

namespace noob
{
	class mixer
	{
		public:
			mixer() noexcept(true) : current(0.0) {}
			
			float get_sample() const noexcept(true)
			{
			}

			void mix_sample(float s) const noexcept(true)
			{
			}

			void next_sample() const noexcept(true)
			{
			}

		protected:
			float current;
	}
}
