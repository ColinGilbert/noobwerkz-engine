#pragma once

#include <memory>

#include "Drawable.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "TransformHelper.hpp"

namespace noob
{
	class actor
	{
		public:
		actor() : hits(100.0), base_damage(1.0), drawable_id(0), name("") {}
	
		float hits, base_damage;
		noob::transform_helper xform;
		size_t drawable_id;
		std::string name;
	};
}
