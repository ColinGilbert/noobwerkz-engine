#pragma once

#include <vector>
#include <algorithm>

namespace noob
{
	template <typename T>
		class chunked_ringbuffer
		{
			public:
				chunked_ringbuffer() noexcept(true) : chunk_size(0), num_chunks(0), head_r(0), tail_w(0) {}

				// Invalidates everything. Use case assumes a single producer and N consumers. Will loop back to beginning of buffer on overread/overwrite.
				void resize(size_t num_chunks_arg, size_t chunk_size_arg) noexcept(true)
				{
					num_chunks = num_chunks_arg;
					chunk_size = chunk_size_arg;
					elems.resize(num_chunks * chunk_size);
					head_r = tail_w = 0;					
				}
				
				const T* head() const noexcept(true)
				{
					return &(elems[head_r]);
				}

				T* tail() noexcept(true)
				{
					return &(elems[tail_w]);
				}

				void push() noexcept(true)
				{	
					tail_w += chunk_size;
					tail_w %= (num_chunks * chunk_size);
				}

				void pop() noexcept(true)
				{
					head_r += chunk_size;
					head_r %= (num_chunks * chunk_size);
				}
			
				void fill(T arg) noexcept(true)
				{
					std::fill(elems.begin(), elems.end(), arg);
				}

				size_t size() const noexcept(true)
				{
					return chunk_size;
				}
			
				size_t chunks() const noexcept(true)
				{
					return num_chunks;
				}

			protected:
				std::vector<T> elems;
				size_t chunk_size, num_chunks, head_r, tail_w;
			};
}
