#pragma once

#include "Actor.hpp"


namespace noob
{
	struct actor_blueprints
	{
		uint16_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale, los;
		noob::shape_handle bounds;
		noob::model_handle model;
		noob::reflectance_handle reflect;
		// noob::armature_handle arma;
		std::string to_string() const noexcept(true)
		{
			return noob::concat("shape ", noob::to_string(bounds.index()), ", model ", noob::to_string(model.index()) ,", reflection ", noob::to_string(reflect.index()), /* ", armature ", noob::to_string(arma.index()) , */ ", strength ", noob::to_string(strength), ", range ", noob::to_string(range), ", defense ", noob::to_string(defense), ", move speed ", noob::to_string(movement_speed), ", attack speed ", noob::to_string(attack_speed), ", stamina ", noob::to_string(stamina), ", morale ", noob::to_string(morale), ", LOS ", noob::to_string(los));
		}
	};

	typedef noob::component<noob::actor_blueprints> actor_blueprints_holder;
	typedef noob::handle<noob::actor_blueprints> actor_blueprints_handle;
}
