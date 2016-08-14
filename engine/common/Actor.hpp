#pragma once

#include <rdestl/fixed_array.h>


#include <limits>

#include "format.h"

#include "Shape.hpp"
#include "Component.hpp"
#include "ComponentDefines.hpp"
#include "ScaledModel.hpp"
#include "Armature.hpp"
#include "ShadingVariant.hpp"
#include "NoobCommon.hpp"
#include "StageTypes.hpp"

namespace noob
{
	struct actor_blueprints
	{
		uint16_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale, los;
		// noob::armature_handle arma;
		noob::shape_handle bounds;
		// noob::surface drawing;
		noob::shader shader;
		noob::reflectance_handle reflect;

		std::string to_string() const noexcept(true)
		{
			fmt::MemoryWriter ww;
			ww << "shape " << bounds.index() << ", shader " << shader.to_string() << ", reflection " << reflect.index() << ", strength " << strength << ", range " << range << ", defense " << defense << ", move speed " << movement_speed << ", attack speed " << attack_speed << ", stamina " << stamina << ", morale " << morale << ", LOS " << los;
			return ww.str();
		}
	};


	struct actor
	{
		actor() noexcept(true) : alive(true), stance(noob::actor::stance_type::STANDING), activity(noob::actor::activity_type::GUARDING), mentality(noob::actor::mentality_type::ALERT), velocity(noob::vec3(0.0, 0.0, 0.0)), incline(0.0) {}
		
		// These are basic stances upon which all other activities are animated.
		enum class stance_type : uint16_t { STANDING = 0, MOUNTED = 4, SITTING = 5, KNEELING = 6, PRONE = 7, DOWN = 8 };

		enum class activity_type : uint16_t { GUARDING = 0, CHARGING = 1, BRACING = 2, FIGHTING = 3, SHOOTING = 4, RELOADING = 5, TAUNTING = 6, DEAD = 7, PRAYING = 8, SALUTING = 9, WORKING = 10, CELEBRATING = 11, SLEEPING = 12 };

		enum class mentality_type : uint16_t { ALERT = 0, AGGRESSIVE = 1, DEFENSIVE = 2, PANIC = 3, RELAXED = 4, UNCONSCIOUS = 5 };

		static const noob::stage_item_type type = noob::stage_item_type::ACTOR;
		bool alive;

		noob::actor::stance_type stance;
		noob::actor::activity_type activity;
		noob::actor::mentality_type mentality;

		noob::ghost_handle ghost;
		noob::vec3 velocity, target_pos;
		float incline;
	};

	struct scenery
	{
		//constexpr noob::stage_item::type type = 
		noob::body_handle body;
		noob::shader shader;
		noob::reflectance_handle reflect;
	};

	typedef noob::component<noob::actor_blueprints> actor_blueprints_holder;
	
	typedef noob::handle<noob::actor_blueprints> actor_blueprints_handle;
	typedef noob::handle<noob::actor> actor_handle;
	typedef noob::handle<noob::scenery> scenery_handle;


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
