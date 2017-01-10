#pragma once

#include <rdestl/fixed_array.h>

#include <limits>

#include "Shape.hpp"
#include "ComponentDefines.hpp"
#include "StageItemType.hpp"

namespace noob
{
	struct scenery
	{
		scenery() noexcept(true) {}
		static const noob::stage_item_type type = noob::stage_item_type::SCENERY;
		noob::body_handle body;
		// noob::model_handle model;
		// noob::reflectance_handle reflect;
		// uint8_t flag;
	};

	typedef noob::handle<noob::scenery> scenery_handle;
}
