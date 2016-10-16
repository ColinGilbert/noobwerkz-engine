#pragma once

#include "StringFuncs.hpp"

namespace noob
{
	enum class shader_type
	{       
		BASIC = 0,
		TRIPLANAR = 1
	};

	struct shader_variant
	{
		shader_variant() noexcept(true) : type(shader_type::BASIC), handle(0) {}

		bool operator==(const noob::shader_variant& other) const noexcept(true)
		{
			return (type == other.type && handle == other.handle);
		}

		shader_type type;
		uint32_t handle;

		std::string to_string() const
		{

			std::string results;
			switch (type)
			{
				case (noob::shader_type::BASIC):
					{
						results += "basic ";
						break;
					}
				case (noob::shader_type::TRIPLANAR):
					{
						results += "triplanar ";
						break;
					}
			}

			return noob::concat(results, noob::to_string(handle));

		}
	};
}
