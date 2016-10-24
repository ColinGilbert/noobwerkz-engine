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

		bool push_back(const noob::vec4& arg) noexcept(true)
		{
			if ((current + sizeof(arg)) <= size)
			{
				noob::vec4* ptr = reinterpret_cast<noob::vec4*>(raw_buf[current]);
				*ptr = arg;
				current += sizeof(arg);
				return true;
			}
			else
			{
				return false;
			}
		}

		bool push_back(const noob::mat4& arg) noexcept(true)
		{
			if ((current + sizeof(arg)) <= size)
			{
				noob::mat4* ptr = reinterpret_cast<noob::mat4*>(raw_buf[current]);
				*ptr = arg;
				current += sizeof(arg);
				return true;
			}
			else
			{
				return false;
			}
		}

		static noob::gpu_write_buffer invalid() noexcept(true)
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
