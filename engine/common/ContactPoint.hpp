#pragma once

#include "StageItemType.hpp"
#include "StringFuncs.hpp"

namespace noob
{
	struct contact_point
	{
		noob::stage_item_type item_type;
		uint32_t index;
		noob::vec3f pos_a, pos_b, normal_on_b;

		static std::string to_string(const noob::contact_point& cp) noexcept(true)
		{
			return noob::concat(noob::to_string(cp.item_type), " at index ", noob::to_string(cp.index), ", pos a", noob::to_string(cp.pos_a), ", pos b", noob::to_string(cp.pos_b), ", normal on b", noob::to_string(cp.normal_on_b));
		}
	};
}
