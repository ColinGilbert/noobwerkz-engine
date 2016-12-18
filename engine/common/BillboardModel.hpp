#pragma once

#include <noob/component/component.hpp>

#include "Model.hpp"

namespace noob
{
	// In this engine, billboards are textured 2D items that get shown in screen-space and drawn en-masse.
	struct billboard_buffer : public model_buffered
	{
		static constexpr uint32_t stride = sizeof(noob::vec2f) * 2;
	};

	typedef noob::handle<noob::billboard_buffer> billboard_buffer_handle;

}
