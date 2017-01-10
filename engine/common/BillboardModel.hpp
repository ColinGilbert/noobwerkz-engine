#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	// In this engine, billboards are textured 2D items that get shown in screen-space and drawn en-masse.
	struct billboard_buffer : public model_buffered
	{
		static constexpr uint32_t stride = sizeof(vec4f) * 2;
	};

	struct billboard_vertex
	{
		noob::vec2f pos;
		noob::vec2f uv;
		noob::vec4f colour;
	};

	typedef noob::handle<noob::billboard_buffer> billboard_buffer_handle;

}
