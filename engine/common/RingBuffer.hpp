#pragma once

#include <atomic>
#include <vector>

namespace noob
{
	template <typename T> class ringbuffer
	{
		public:
			ringbuffer() noexcept(true) : current(0), offset(0) {}

			T& operator[](size_t i)
			{
				return elems[current * offset + i];
			}

			void swap()
			{
				current ^= 1; // Switch buffer
			}

			size_t size()
			{
				return offset;
			}
	
			void resize(size_t num)
			{
				size_t old_size = elems.size();
				if (old_size >= num * 2)
				{
					return;
				}
				std::vector<T> old_elems(elems);
				size_t old_offset = offset;
				offset = num;
			
				elems.clear();
				elems.resize(num * 2);
	
				for (size_t i = 0; i < old_offset; ++i)
				{
					elems[i] = old_elems[i];
				}

				for (size_t i = old_offset; i < old_elems.size(); ++i)
				{
					elems[offset + i] = old_elems[old_offset + i];
				}

			}
		private:
			std::vector<T> elems;
			std::atomic<size_t> current;
			size_t offset;
	};
}
