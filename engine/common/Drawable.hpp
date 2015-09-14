// Interface class for drawble items
#pragma once

#include <bgfx.h>
#include "MathFuncs.hpp"

namespace noob
{
	class drawable
	{
		public:
			virtual void draw(uint8_t view_id, const noob::mat4& animated_model_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const = 0;
	};
}
