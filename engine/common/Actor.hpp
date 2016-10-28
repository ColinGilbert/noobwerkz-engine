#pragma once

#include <rdestl/fixed_array.h>

#include <limits>

#include "Shape.hpp"
#include "ComponentDefines.hpp"
#include "StageItemType.hpp"
#include "Armature.hpp"


namespace noob
{
	struct actor_blueprints
	{
		uint16_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale, los;
		noob::shape_handle bounds;
		noob::model_handle model;
		noob::reflectance_handle reflect;
		noob::armature_handle arma;
		bool needs_colours;//, needs_matrices;
		std::string to_string() const noexcept(true)
		{
			return noob::concat("shape ", noob::to_string(bounds.index()), ", model ", noob::to_string(model.index()) ,", reflection ", noob::to_string(reflect.index()), ", armature ", noob::to_string(arma.index()) ,", strength ", noob::to_string(strength), ", range ", noob::to_string(range), ", defense ", noob::to_string(defense), ", move speed ", noob::to_string(movement_speed), ", attack speed ", noob::to_string(attack_speed), ", stamina ", noob::to_string(stamina), ", morale ", noob::to_string(morale), ", LOS ", noob::to_string(los));
		}
	};

	typedef noob::component<noob::actor_blueprints> actor_blueprints_holder;
	typedef noob::handle<noob::actor_blueprints> actor_blueprints_handle;

	struct actor
	{
		actor() noexcept(true) : alive(true), team(0), pose(0), velocity(noob::vec3(0.0, 0.0, 0.0)), target_pos(noob::vec3(0.0, 0.0, 0.0)), incline(0.0), gravity_coeff(1.0) {}

		// These are basic stances upon which all other activities are animated.
		// 
		// enum class stance_type : uint16_t { STAND = 0, MOUNT = 1, SIT = 2, KNEEL = 3, PRONE = 4, KO = 5 };

		// enum class activity_type : uint16_t { GUARDING = 0, CHARGING = 1, BRACING = 2, MELEE = 3, SHOOTING = 4, RELOADING = 5, TAUNTING = 6, DEAD = 7, PRAYING = 8, SALUTING = 9, WORKING = 10, CELEBRATING = 11, SLEEPING = 12 };

		// enum class mentality_type : uint16_t { ALERT = 0, AGGRESSIVE = 1, DEFENSIVE = 2, PANIC = 3, RELAXED = 4, UNCONSCIOUS = 5 };

		static const noob::stage_item_type type = noob::stage_item_type::ACTOR;

		bool alive;
		uint32_t team, pose;
		noob::actor_blueprints_handle bp_handle;
		noob::ghost_handle ghost;
		noob::vec3 velocity, target_pos;
		float incline, gravity_coeff;

		// noob::actor::stance_type stance;
		// noob::actor::activity_type activity;
		// noob::actor::mentality_type mentality;
	};

	typedef noob::handle<noob::actor> actor_handle;

	// Events play a dual role: They can either be commands to an actor, or requests to make an actor interact with the world.
	// Upon being fired off, events are pushed onto a pre-allocated stack.
	// The stack is traversed at regular intervals and the the AI filters out the decisions to be made. This allows for individual events not to disrupt simulation consistency.
	struct alignas(16) actor_event
	{
		// noob::actor_event::action todo;
		noob::actor_handle from, to;
		uint32_t index;
	};
}
