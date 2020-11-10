#pragma once

#include <noob/component/component.hpp>
#include <rdestl/fixed_array.h>

#include <limits>

#include "Shape.hpp"
#include "StageItemType.hpp"
#include "Armature.hpp"
#include "ActorBlueprints.hpp"

namespace noob
{

	struct actor
	{
		actor() noexcept(true) : self_control(true), team(0), pose(0), velocity(noob::vec3f(0.0, 0.0, 0.0)) /*, target_pos(noob::vec3f(0.0, 0.0, 0.0)), incline(0.0), gravity_coeff(1.0) */ {}

		// These are basic stances upon which all other activities are animated.
		// 
		enum class stance_type : uint16_t { STAND = 0, MOUNT = 1, SIT = 2, KNEEL = 3, PRONE = 4, KO = 5 };

		enum class activity_type : uint16_t { GUARDING = 0, CHARGING = 1, BRACING = 2, MELEE = 3, SHOOTING = 4, RELOADING = 5, TAUNTING = 6, DEAD = 7, PRAYING = 8, SALUTING = 9, WORKING = 10, CELEBRATING = 11, SLEEPING = 12 };

		enum class mentality_type : uint16_t { ALERT = 0, AGGRESSIVE = 1, DEFENSIVE = 2, PANIC = 3, RELAXED = 4, UNCONSCIOUS = 5 };

		static constexpr noob::stage_item_type type = noob::stage_item_type::ACTOR;

		bool self_control;
		uint32_t team, pose;
		noob::actor_blueprints_handle bp_handle;
		noob::ghost_handle ghost;
		noob::vec3f velocity, target_pos;
		// float incline, gravity_coeff;

		noob::actor::stance_type stance;
		noob::actor::activity_type activity;
		noob::actor::mentality_type mentality;
	};

	typedef noob::handle<noob::actor> actor_handle;
/*
	// Events play a dual role: They can either be commands to an actor, or requests to make an actor interact with the world.
	// Upon being fired off, events are pushed onto a pre-allocated stack.
	// The stack is traversed at regular intervals and the the AI filters out the decisions to be made. This allows for individual events not to disrupt simulation consistency.
	struct alignas(16) actor_event
	{
		// noob::actor_event::action todo;
		noob::actor_handle from, to;
		uint32_t index;
	};
*/
}
