#pragma once

#include "BodyInfo.hpp"
#include "Reflectance.hpp"
#include "Colour.hpp"

namespace noob
{
	struct prop
	{
		noob::body_handle bod;
		noob::colourfp_handle colour;
	};

	typedef noob::component<noob::prop> prop_holder;
	typedef noob::handle<noob::prop> prop_handle;
/*
	static std::string to_string(const noob::prop_blueprints Blueprints) noexcept(true)

	{
		//return noob::concat("shape ", noob::to_string(Blueprints.bounds.index()), ", model ", noob::to_string(Blueprints.model.index()) ,", reflection ", noob::to_string(Blueprints.reflect.index()), ", strength ", noob::to_string(Blueprints.strength), ", range ", noob::to_string(Blueprints.range), ", defense ", noob::to_string(Blueprints.defense), ", move speed ", noob::to_string(Blueprints.movement_speed), ", attack speed ", noob::to_string(Blueprints.attack_speed), ", stamina ", noob::to_string(Blueprints.stamina), ", morale ", noob::to_string(Blueprints.morale), ", LOS ", noob::to_string(Blueprints.los));
	}
*/
}
