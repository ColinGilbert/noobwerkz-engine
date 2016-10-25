#pragma once

#include <rdestl/fixed_array.h>

#include <limits>

#include "Shape.hpp"
#include "ComponentDefines.hpp"
#include "StageItemType.hpp"

namespace noob
{
	struct scenery_blueprints
	{
		noob::shape_handle bounds;
		noob::reflectance_handle reflect;

		std::string to_string() const noexcept(true)
		{
			 return noob::concat("shape ", noob::to_string(bounds.index()), ", reflection ", noob::to_string(reflect.index()));
		}
	};


	struct scenery
	{
		scenery() noexcept(true) : flag(0) {}
		static const noob::stage_item_type type = noob::stage_item_type::SCENERY;
		noob::body_handle body;
		noob::reflectance_handle reflect;
		uint8_t flag;
	};

	typedef noob::handle<noob::scenery> scenery_handle;
	
}
