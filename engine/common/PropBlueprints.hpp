#pragma once

#include "Prop.hpp"
#include "Model.hpp"
#include "Reflectance.hpp"
#include "CompoundShape.hpp"

namespace noob
{
	struct prop_blueprints
	{
		noob::shape_handle shape;
		noob::instanced_model_handle model;
		noob::reflectance_handle reflect;
		float mass;//, friction, restitution;
		bool ccd;
	};

	typedef noob::handle<noob::prop_blueprints> prop_blueprints_handle;

	//static std::string to_string(const noob::prop_blueprints Blueprints) noexcept(true)
	//{
	//	return noob::concat("shape ", noob::to_string(Blueprints.bounds.index()), ", model ", noob::to_string(Blueprints.model.index()) ,", reflection ", noob::to_string(Blueprints.reflect.index()), /* ", armature ", noob::to_string(arma.index()) , */ ", strength ", noob::to_string(Blueprints.strength), ", range ", noob::to_string(Blueprints.range), ", defense ", noob::to_string(Blueprints.defense), ", move speed ", noob::to_string(Blueprints.movement_speed), ", attack speed ", noob::to_string(Blueprints.attack_speed), ", stamina ", noob::to_string(Blueprints.stamina), ", morale ", noob::to_string(Blueprints.morale), ", LOS ", noob::to_string(Blueprints.los));
	// }

}
