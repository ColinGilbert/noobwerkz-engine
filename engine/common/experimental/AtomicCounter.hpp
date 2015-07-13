#pragma once

#include <atomic>

namespace noob
{
	class atomic_reference_counter
	{
		public:
			

			atomic_counter() : count(0) {}
			size_t increment()
			{
				++count;
				return count;
			}

			size_t decrement()
			{
				--count;
				return count;
			}

		protected:
			std::atomic<size_t> count;
	};

}
