// Interface class for drawble items
#pragma once

#include <bgfx/bgfx.h>
#include "MathFuncs.hpp"

namespace noob
{
	class drawable
	{
		public:
			virtual void draw(uint8_t view_id, const noob::mat4& model_mat, const noob::mat4& normal_mat, const bgfx::ProgramHandle& prog, uint64_t bgfx_state_flags = BGFX_STATE_DEFAULT) const = 0;
	};
}
