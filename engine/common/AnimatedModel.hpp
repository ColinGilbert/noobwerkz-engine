// This class represents a humanoid skeletal blend-tree
#pragma once

#include <unordered_map>

#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/base/io/archive.h>
#include <ozz/base/io/stream.h>

#include <cereal/types/string.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/archives/binary.hpp>

#include <lemon/static_graph.h>

namespace noob
{
	class animated_model
	{
		public:
			//animated_model() : blend_weights(blend_tree) {}
			
			enum class posture_type
			{
				STANDING, CROUCHING, PRONE, SITTING, RIDING, BRACING
			};
			
			enum class equipment_type
			{
				UNARMED, SHIELD, SWORD, CLUB, SPEAR, PIKE, BOW
			};
			
			enum class movement_type
			{
				STATIONARY, WALK, RUN, CREEP
			};
			
			enum class movement_direction
			{
				FORWARD, LEFT, RIGHT, BACKWARD
			};
			
			enum class action_type
			{
				IDLE, MELEE, SHOOT, RELOAD, SALUTE, INSULT, COVER, JUMP
			};
			
			struct animation_type
			{
				noob::animated_model::equipment_type gear;
				noob::animated_model::posture_type posture;
				noob::animated_model::movement_type movement;
				noob::animated_model::movement_direction direction;
				noob::animated_model::action_type action;
			};
			
			void load_mesh(const std::string& filename);
			void load_skeletion(const std::string& filename);
			// void load_skeleton(const ozz::animation::raw_skeleton& skel);
			// These functions work as a stack: You set the current stance, posture, movement.
			// bool load_animation(const ozz::animation::raw_animation& anim);
			
			void current_posture(noob::animated_model::posture_type current_posture);
			void current_equipment(noob::animated_model::equipment_type current_equipment);
			void current_movement(noob::animated_model::movement_type current_movement);
			void current_direction(noob::animated_model::movement_direction current_direction);
			void current_action(noob::animated_model::action_type current_action);
			
			// Returns if current state is valid. For example, jumping and lying on the ground at the same time is quite difficult...
			bool current_state_valid() const;
			
			// Return value: <from_anim, to_anim, normalized_blend>
			std::tuple<noob::animated_model::animation_type,noob::animated_model::animation_type,float> get_current_anim() const;
			bool set_default(const noob::animated_model::animation_type& anim);
			bool anim_exists(noob::animated_model::animation_type anim) const;
			
			// If the desired animation isn't present, this will go to the most specifid item in the blend tree.
			void switch_to(noob::animated_model::animation_type anim, float fade);
			
			// When there is a filename in the function call, the animation is set as the ozz-animation file.
			//void weapon(noob::animated_model::equipment_type weapon_state, const std::string& filename = "");
			//void action(noob::animated_model::action_type action_state, const std::string& filename = "");
			
		protected:
			//std::vector<noob::drawable3d> drawables;
			ozz::animation::Skeleton skeleton;
			// std::unordered_map<std::string, ozz::animation::Animation> animations;
			lemon::StaticDigraph blend_tree;
			lemon::StaticDigraph::ArcMap<float> blend_weights;
			
			//lemon::StaticDigraph::NodeMap<noob::animated_model::anim_state> blend_states;
	};
}
