#pragma once

// External libs
#include <noob/math/math_funcs.hpp>

namespace noob
{
	class graphics;

	class gpu_write_buffer
	{
		friend class graphics;

		public:

		bool valid() const noexcept(true)
		{
			return raw_buf != nullptr;
		}

		bool push_back(const noob::vec4& arg) noexcept(true)
		{
			if (current_pos < size - 3)
			{
				for (float f : arg.v)
				{
					raw_buf[current_pos] = f;
					++current_pos;
				}
				return true;
			}
			else
			{
				return false;
			}
		}

		bool push_back(const noob::mat4& arg) noexcept(true)
		{
			if (current_pos < size - 15)
			{
				for (float f : arg.m)
				{
					raw_buf[current_pos] = f;
					++current_pos;
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
		gpu_write_buffer(float* raw_buf_arg, uint32_t size_arg) : current_pos(0), size(size_arg), raw_buf(raw_buf_arg) {}

		uint32_t current_pos;
		const uint32_t size;
		float* raw_buf;
	};
}
