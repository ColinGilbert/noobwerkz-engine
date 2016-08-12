#pragma once

#include <rdestl/fixed_array.h>
#include <limits>
#include "Shape.hpp"
#include "Component.hpp"
#include "ComponentDefines.hpp"
#include "ScaledModel.hpp"
#include "Armature.hpp"
#include "ShadingVariant.hpp"

namespace noob
{
	struct actor_blueprints
	{
		uint16_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale, los;
	//	noob::armature_handle arma;
		noob::shape_handle bounds;
		// noob::surface drawing;
		noob::shader shader;
		noob::reflectance_handle reflect;
	};

	typedef noob::component<noob::actor_blueprints> actor_blueprints_holder;
	typedef noob::handle<noob::actor_blueprints> actor_blueprints_handle;

	struct actor
	{
		actor() noexcept(true) : alive(true), stance(noob::actor::stance_type::STANDING), activity(noob::actor::activity_type::GUARDING), mentality(noob::actor::mentality_type::ALERT), velocity(noob::vec3(0.0, 0.0, 0.0)), incline(0.0) {}


		// These are basic stances upon which all other activities are animated.
		enum class stance_type : uint16_t { STANDING = 0, MOUNTED = 4, SITTING = 5, KNEELING = 6, PRONE = 7, DOWN = 8 };

		// These activities consist of basic loops. Additive blending is then used to quickly animate our little fellows.
		enum class activity_type : uint16_t { GUARDING = 0, CHARGING = 1, BRACING = 2, FIGHTING = 3, SHOOTING = 4, RELOADING = 5, TAUNTING = 6, DEAD = 7, PRAYING = 8, SALUTING = 9, WORKING = 10, CELEBRATING = 11, SLEEPING = 12 };

		// Mental states affect our little people in both positive and negative ways.
		// Alert is the default mental state. Full LOS and no other modifiers.
		// Aggressive means more hand-to-hand damage given, plus attack speed increase. LOS 85%.
		// Defensive reduces damage taken. LOS 85%
		// Panic increases all movement and attack speeds, hand-to-hand damage given, and all damage taken. Reduces shooting accuracy. LOS 85%. Also, reduced command efficacy.
		// Relaxed increases damage taken. LOS 75%.
		// Unconscious greatly increases damage taken, reduces speed and damage given to 0. No LOS.
		enum class mentality_type : uint16_t { ALERT = 0, AGGRESSIVE = 1, DEFENSIVE = 2, PANIC = 3, RELAXED = 4, UNCONSCIOUS = 5 };






		bool alive;

		noob::actor::stance_type stance;
		noob::actor::activity_type activity;
		noob::actor::mentality_type mentality;

		noob::ghost_handle ghost;
		noob::vec3 velocity, target_pos;
		float incline;

	};

	typedef noob::component<noob::actor> actor_holder;
	typedef noob::handle<noob::actor> actor_handle;

	// Events play a dual role: They can either be commands to an actor, or requests to make an actor interact with the world.
	// Upon being fired off, events are pushed onto a pre-allocated stack.
	// The stack is traversed at regular intervals and the the AI filters out the decisions to be made. This allows for individual events not to disrupt simulation consistency.
	struct alignas(16) actor_event
	{

	//	noob::actor_event::action todo;
		noob::actor_handle from, to;
		uint32_t index;
	};
}
