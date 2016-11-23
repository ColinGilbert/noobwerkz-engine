#pragma once

#include <atomic>
#include <vector>

namespace noob
{
	template <typename T> class ringbuffer
	{
		public:
			class reader
			{
				T operator[](size_t i) const noexcept(true) { return ptr[i]; }

				private:
				reader(size_t chunk, size_t chunk_size) noexcept(true) : ptr(&elems[chunk*chunk_size]) {}				
				const T* ptr;
			};

			struct writer
			{
				T& operator[](size_t i) noexcept(true) { return ptr[i]; }

				private:
				writer(size_t chunk, size_t chunk_size) noexcept(true) : ptr(&elems[chunk*chunk_size]) {}				
				T* ptr;
			};
			
			ringbuffer() noexcept(true) = default;
			ringbuffer(size_t num_chunks, size_t chunk_size) noexcept(true) : m(chunk_size, num_chunks, 0, 0) {}

			void push() noexcept(true)
			{
				++(m.current_chunk_w);
			}

			void pop() noexcept(true)
			{
				++(m.current_chunk_r);
			}
			
			reader get_reader(size_t chunk) const noexcept(true)
			{
				return reader(chunk, m.chunk_size);
			}

			// Fun fact: This function is marked const but the item returned itself can definitely change the values of our data structure. In fact, readers/writers aren't even bounds-checked!
			writer get_writer(size_t chunk) const noexcept(true)
			{
				return writer(chunk, m.chunk_size);			
			}

			size_t chunk_size() const noexcept(true)
			{
				return m.chunk_size;
			}

			size_t total_size() const noexcept(true)
			{
				return m.total_size;
			}

			// Resize invalidates everything.
			void resize(size_t num_chunks, size_t chunk_size) noexcept(true)
			{
				m.num_chunks = num_chunks;
				m.chunk_size = chunk_size;
				m.current_chunk_r = m.current_chunk_w = 0;
				m.total_size = m.num_chunks * m.chunk_size;
			}

			void fill_all(T t) noexcept(true)
			{
				for (size_t i = 0; i < elems.size(); ++i)
				{
					elems[i] = t;
				}
			}

		private:
			
			struct members
			{
				members(size_t _chunk_size, size_t _num_chunks, size_t _current_chunk_r, size_t _current_chunk_w, size_t _total_size) noexcept(true) : chunk_size(_chunk_size), num_chunks(_num_chunks), current_chunk_r(_current_chunk_r), current_chunk_w(_current_chunk_w), total_size(_total_size) {}
				members() noexcept(true) = default;

				size_t chunk_size, num_chunks, current_chunk_r, current_chunk_w, total_size;
			};

			members m;

			std::vector<T> elems;
	};
}
