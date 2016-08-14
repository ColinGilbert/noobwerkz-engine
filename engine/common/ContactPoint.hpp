#pragma once

#include "StageTypes.hpp"

namespace noob
{
	struct contact_point
	{
		noob::stage_item_type item_type;
		uint32_t index;
		noob::vec3 pos_a, pos_b, normal_on_b;

		std::string to_string() const noexcept(true)
		{
			fmt::MemoryWriter ww;
			ww << noob::to_string(item_type) << " at index " << index << ", pos a" << pos_a.to_string() << ", pos b" << pos_b.to_string() << ", normal on b" << normal_on_b.to_string();
			return ww.str();
		}
	};
}
