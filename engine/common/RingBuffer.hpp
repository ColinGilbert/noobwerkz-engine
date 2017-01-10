#pragma once

#include <vector>

namespace noob
{
	template <typename T> class ringbuffer
	{
		public:
	
			void swap() noexcept(true)
			{
				T* temp = head_ptr;
				head_ptr = tail_ptr;
				tail_ptr = temp;
			}
			
			const T* head() const noexcept(true)
			{
				return head_ptr;
			}

			T* tail() const noexcept(true)
			{
				return tail_ptr;
			}

			size_t size() const noexcept(true)
			{
				return chunk_size;
			}

			// Resize invalidates everything.
			void resize(size_t arg) noexcept(true)
			{
				chunk_size = arg;
				elems.resize(arg * 2);
				head_ptr = &elems[0];
				tail_ptr = &elems[arg];
			}

			void fill(T t) noexcept(true)
			{
				for (size_t i = 0; i < elems.size(); ++i)
				{
					elems[i] = t;
				}
			}

		private:
			
			T *head_ptr, *tail_ptr;	
			size_t chunk_size;
			std::vector<T> elems;
	};
}
