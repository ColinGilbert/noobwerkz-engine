#pragma once

#include "Actor.hpp"


namespace noob
{
	struct actor_blueprints
	{
		uint16_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale, los;
		noob::shape_handle bounds;
		// TODO: replace with armature
		noob::instanced_model_handle model;
		noob::reflectance_handle reflect;

	};

	typedef noob::component<noob::actor_blueprints> actor_blueprints_holder;
	typedef noob::handle<noob::actor_blueprints> actor_blueprints_handle;

	static std::string to_string(const noob::actor_blueprints Blueprints) noexcept(true)
	{
		return noob::concat("shape ", noob::to_string(Blueprints.bounds.index()), ", model ", noob::to_string(Blueprints.model.index()) ,", reflection ", noob::to_string(Blueprints.reflect.index()), /* ", armature ", noob::to_string(arma.index()) , */ ", strength ", noob::to_string(Blueprints.strength), ", range ", noob::to_string(Blueprints.range), ", defense ", noob::to_string(Blueprints.defense), ", move speed ", noob::to_string(Blueprints.movement_speed), ", attack speed ", noob::to_string(Blueprints.attack_speed), ", stamina ", noob::to_string(Blueprints.stamina), ", morale ", noob::to_string(Blueprints.morale), ", LOS ", noob::to_string(Blueprints.los));
	}

}
