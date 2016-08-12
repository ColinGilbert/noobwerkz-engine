#pragma once

#include "Component.hpp"
#include "Reflectance.hpp"

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
	};

	struct surface
	{
		shader_variant shading;
		noob::handle<noob::reflectance> reflect;
	};
};
