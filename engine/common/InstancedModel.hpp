#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	struct instanced_model
	{
		static constexpr uint32_t materials_stride = sizeof(noob::vec4f);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4f) * 2;

		noob::model_indexed base;
		uint32_t colours_vbo, matrics_vbo;
	};

	typedef noob::handle<noob::instanced_model> instanced_model_handle;
}
