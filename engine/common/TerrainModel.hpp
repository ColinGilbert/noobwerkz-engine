#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	struct terrain_model
	{
		static constexpr uint32_t stride = sizeof(noob::vec4f) * 3;
		noob::model_basic base;
	};

	typedef noob::handle<noob::terrain_model> terrain_model_handle;
}
