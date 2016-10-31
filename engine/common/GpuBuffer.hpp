#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class graphics;

	class gpu_write_buffer
	{
		friend class graphics;

		public:

		// TODO:
		// Templatize!

		bool valid() const noexcept(true)
		{
			return raw_buf != nullptr;
		}

		bool push_back(const noob::vec4 arg) noexcept(true)
		{
			if (current < size)
			{
				float* ptr = reinterpret_cast<float*>(raw_buf);
				for (float f : arg.v)
				{
					ptr[current] = f;
					current += sizeof(float);
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		bool push_back(const noob::mat4 arg) noexcept(true)
		{
			if (current < size)
			{
				float* ptr = reinterpret_cast<float*>(raw_buf);
				for (float f : arg.m)
				{
					ptr[current] = f;
					current += sizeof(float);
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		static noob::gpu_write_buffer make_invalid() noexcept(true)
		{
			return noob::gpu_write_buffer(nullptr, 0);
		}

		protected:
		gpu_write_buffer(uint8_t* raw_buf_arg, uint32_t size_arg) : current(0), size(size_arg), raw_buf(raw_buf_arg) {}

		uint32_t current;
		const uint32_t size;
		uint8_t* raw_buf;
	};
}
