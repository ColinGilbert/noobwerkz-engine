#pragma once

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "format.h"

namespace noob
{
	enum class stage_item_type : uint32_t
	{
		ACTOR = 0, SCENERY = 1, TRIGGER = 2, LIGHT = 3, PARTICLES = 4
	};

	static std::string to_string(stage_item_type t)
	{
		switch (t)
		{
			case (stage_item_type::ACTOR):
			{
				return "actor";
			}
			case (stage_item_type::SCENERY):
			{
				return "scenery";
			}
			case (stage_item_type::TRIGGER):
			{
				return "trigger";
			}
			case (stage_item_type::LIGHT):
			{
				return "light";
			}
			case (stage_item_type::PARTICLES):
			{
				return "particles";
			}
			default:
			{
				return "invalid";
			}
		}
	}
}
