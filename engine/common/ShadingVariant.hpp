#pragma once

#include "format.h"

namespace noob
{
	enum class shader_type
	{       
		BASIC = 0,
		TRIPLANAR = 1
	};

	struct shader
	{
		shader() noexcept(true) : type(shader_type::BASIC), handle(0) {}
		bool operator==(const noob::shader& other) const noexcept(true)
		{
			return (type == other.type && handle == other.handle);
		}
		shader_type type;
		uint32_t handle;

		std::string to_string() const
		{
			fmt::MemoryWriter ww;

			switch (type)
			{
				case (noob::shader_type::BASIC):
					{
						ww << "basic ";
						break;
					}
				case (noob::shader_type::TRIPLANAR):
					{
						ww << "triplanar ";
						break;
					}
			}

			ww << handle;

			return ww.str();

		}
	};
};
