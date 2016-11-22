#pragma once

#include <atomic>
#include <vector>

namespace noob
{
	template <typename T> class ringbuffer
	{
		public:
			ringbuffer() noexcept(true) : first(true), current(0), offset(0) {}

			T& operator[](size_t i)
			{
				return elems[current + i];
			}

			T operator[](size_t i) const
			{
				return elems[current + i];
			}

			void swap()
			{
				if (first)
				{
					current = offset;
					first = false;
				}
				else
				{
					current = 0;
					first = true;
				}
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

				size_t old_offset = offset;
				offset = num;

				if (old_offset != 0)
				{
					std::vector<T> old_elems(elems);

					elems.resize(num * 2);
					elems.clear();

					for (size_t i = 0; i < old_offset; ++i)
					{
						elems[i] = old_elems[i];
					}

					for (size_t i = old_offset; i < old_elems.size(); ++i)
					{
						elems[offset + i] = old_elems[old_offset + i];
					}
				}
				else
				{
					elems.resize(num * 2);
				}
			}
		private:
			std::vector<T> elems;
			std::atomic<bool> first;
			size_t offset, current;
	};
}
