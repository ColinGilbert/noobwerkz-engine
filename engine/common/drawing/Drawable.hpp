// Interface class for drawble items
#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class drawable
	{
		public:
			virtual void draw(uint8_t view_id, const noob::mat4& model_mat, const noob::graphics::program_handle prog, uint64_t state_flags) const = 0;
	};
}
