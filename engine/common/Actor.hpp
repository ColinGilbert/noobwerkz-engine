#pragma once

#include <rdestl/fixed_array.h>

#include "Shape.hpp"
#include "Armature.hpp"
#include "Component.hpp"
#include "ComponentDefines.hpp"
#include "NoobCommon.hpp"


namespace noob
{
	struct actor
	{
		// These are basic stances upon which all other activities are animated.
		enum class stance { STANDING = 0, WALKING = 1, JOGGING = 2, RUNNING = 3, MOUNTED = 4, SITTING = 5, KNEELING = 6, PRONE = 7, KO = 8 };

		// These activities consist of basic loops. Additive blending is then used to quickly animate our little fellows.
		enum class activity { GUARDING = 0, CHARGING = 1, BRACING = 2, FIGHTING = 3, SHOOTING = 4, RELOADING = 5, TAUNTING = 6, DEAD = 7, PRAYING = 8, SALUTING = 9, WORKING = 10, CELEBRATING = 11, SLEEPING = 12 };
		
		// Mental states affect our little people in both positive and negative ways.
		// Alert is the default mental state. Full LOS and no other modifiers.
		// Aggressive means more hand-to-hand damage given, plus attack speed increase. LOS 85%.
		// Defensive reduces damage taken. LOS 85%
		// Desperate increases all imovement and attack speeds, hand-to-hand damage given, and all damage taken. Reduces shooting accuracy. LOS 85%.
		// Relaxed increases damage taken. LOS 75%.
		// Unconscious greatly increases damage taken, reduces speed and damage given to 0. No LOS.
		enum class mentality { ALERT = 0, AGGRESSIVE = 1, DEFENSIVE = 2, DESPERATE = 3, RELAXED = 4, UNCONSCIOUS = 5 };
		
		noob::vec3 velocity;
		noob::ghost_handle world_index;
	};


	struct actor_type
	{
		uint32_t strength, range, armour, defense, movement_speed, attack_speed, stamina, morale;
		rde::fixed_array<noob::drawing_info, 32> shading;
		// noob::armature_handle arma;
		noob::shape_handle bounds;
	};
}
