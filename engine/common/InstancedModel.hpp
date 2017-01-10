// Instanced models are each given a colour and and model+MVP matrices via VBO's.

#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	struct instanced_model : public model_indexed
	{
		static constexpr uint32_t colours_stride = sizeof(noob::vec4f);
		static constexpr uint32_t matrices_stride = sizeof(noob::mat4f) * 2;

		uint32_t colours_vbo = 0;
		uint32_t matrices_vbo = 0;
		uint32_t n_instances = 0;
	};

	typedef noob::handle<instanced_model> instanced_model_handle;
}
