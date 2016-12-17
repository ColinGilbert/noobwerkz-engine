#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	struct text_model
	{
		static constexpr uint32_t stride = sizeof(noob::vec2f) * 2;
		noob::model_basic base;
	};

	typedef noob::handle<noob::text_model> text_model_handle;
}
