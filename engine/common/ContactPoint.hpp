#pragma once

#include "StageTypes.hpp"
#include "StringFuncs.hpp"

namespace noob
{
	struct contact_point
	{
		noob::stage_item_type item_type;
		uint32_t index;
		noob::vec3 pos_a, pos_b, normal_on_b;

		std::string to_string() const noexcept(true)
		{
			return noob::concat(noob::to_string(item_type), " at index ", noob::to_string(index), ", pos a", noob::to_string(pos_a), ", pos b", noob::to_string(pos_b), ", normal on b", noob::to_string(normal_on_b));
		}
	};
}
