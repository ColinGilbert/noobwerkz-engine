#pragma once

// #include "Component.hpp"
// #include "Reflectance.hpp"
// #include "ComponentDefines.hpp"


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
	};
};
