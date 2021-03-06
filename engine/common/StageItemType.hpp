#pragma once


namespace noob
{
	enum class stage_item_type : uint32_t
	{
		ACTOR = 0, SCENERY = 1, TRIGGER = 2, LIGHT = 4
	};

	struct stage_item_variant
	{
		stage_item_type type;
		uint32_t index;
	};

	static std::string to_string(stage_item_type t) noexcept(true)
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

			default:
			{
				return "INVALID";
			}
		}
	}
}
