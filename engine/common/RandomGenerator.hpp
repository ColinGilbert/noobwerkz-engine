#pragma once

#include <random>

#include "MathFuncs.hpp"

namespace noob
{
	class random
	{
		public:
			static double get()
			{
				static std::default_random_engine generator;
 				static std::uniform_real_distribution<double> distribution(0.0, 1.0);	

				return distribution(generator);
			}
	};
};
