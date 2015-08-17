// This class represents an animated character. Current only does one animation at a time, in order to learn the system.
#pragma once


#include <unordered_map>
#include <math.h>

#include <ozz/animation/offline/raw_skeleton.h>
#include <ozz/animation/offline/raw_animation.h>
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/animation/runtime/animation.h>
#include <ozz/base/io/archive.h>
#include <ozz/base/io/stream.h>
#include <ozz/base/maths/vec_float.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/soa_transform.h>
#include <ozz/animation/runtime/blending_job.h>
#include <ozz/animation/runtime/sampling_job.h>
#include <ozz/animation/runtime/local_to_model_job.h>
#include <ozz/animation/offline/animation_builder.h>
#include <ozz/animation/offline/animation_optimizer.h>
#include <ozz/base/memory/allocator.h>

#include <cereal/types/string.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/archives/binary.hpp>

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

#include "MathFuncs.hpp"
#include "Graphics.hpp"

namespace noob
{
	class animated_model
	{
		public:

			animated_model() : allocator(ozz::memory::default_allocator()) {}
			~animated_model();
			// Loads a runtime skeleton. Possibly convert to raw skeleton
			bool load_skeleton(const std::string& filename);
			// Loads a raw animation. You then create runtime animations via optimize()
			bool load_animation(const std::string& filename, const std::string& anim_name);
			// TODO: Implement
			bool load_mesh(const std::string& filename);
			// If name = "" all animations get processed. If all the tolerances == 0.0 it doesn't run an optimization pre-pass prior prior to creating runtime animations. 
			void optimize(float translation_tolerance = 0.0, float rotation_tolerance = 0.0, float scale_tolerance = 0.0, const std::string& name = "");
			// TODO: Implement
			void switch_to_anim(const std::string& name);

			// TODO: Implement
			void update(float dt = 0.0);
			void reset_time(float t = 0.0);
			
			bool anim_exists(const std::string& name) const;
			std::string get_current_anim() const;
			// Gets bone matrices.
			std::vector<noob::mat4> get_matrices() const;
			std::array<noob::vec3,4> get_skeleton_bounds() const;

		protected:
			class playback_controller
			{
				public:
					void update(const ozz::animation::Animation& animation, float dt);
					void reset();

					bool paused;
					float time;
					float playback_speed;
			};

			class sampler
			{
				friend class noob::animated_model;

				public:
				void update(float dt);
				// TODO(?) : Return a reference instead of using arguments
				ozz::Range<ozz::math::SoaTransform> get_local_mats() const;
				void get_model_mats(ozz::Range<ozz::math::Float4x4>& models);
				float weight;

				protected:
				sampler() : weight(1.0), cache(nullptr) {}
				noob::animated_model::playback_controller controller;
				ozz::animation::Animation* animation;
				ozz::animation::Skeleton* skeleton;
				ozz::animation::SamplingCache* cache;
				ozz::Range<ozz::math::SoaTransform> locals;

			};

			noob::animated_model::sampler create_sampler(const ozz::animation::Animation& anim);
			void destroy_sampler(noob::animated_model::sampler&);		
			
			std::string current_anim_name;
			ozz::animation::Animation* current_anim;
			float current_time;

			std::unordered_map<std::string, ozz::animation::Animation*> runtime_anims;
			std::unordered_map<std::string, ozz::animation::offline::RawAnimation> raw_anims;
			// float threshold;
			// size_t num_layers;
			// lemon::StaticDigraph blend_tree;
			// ozz::Range<ozz::math::SoaTransform> blended_locals;
			ozz::animation::Skeleton skeleton;
			ozz::Range<ozz::math::Float4x4> model_matrices;
			ozz::memory::Allocator* allocator;
	};
}

/*	
	enum class posture_type
	{
	STANDING, CROUCHING, PRONE, SITTING, RIDING
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
	IDLE, MELEE, SHOOT, RELOAD, SALUTE, INSULT, COVER, JUMP, BRACE
	};

	struct animation_type
	{
	noob::animated_model::equipment_type gear;
	noob::animated_model::posture_type posture;
	noob::animated_model::movement_type movement;
	noob::animated_model::movement_direction direction;
	noob::animated_model::action_type action;
	};
	*/

/*
   void posture(noob::animated_model::posture_type current_posture);
   void equipment(noob::animated_model::equipment_type current_equipment);
   void movement(noob::animated_model::movement_type current_movement);
   void direction(noob::animated_model::movement_direction current_direction);
   void action(noob::animated_model::action_type current_action);
   bool noob::animated_model::load_animation(const std::string& filename, noob::animation_type anim)
   */
// Returns if current state is valid. For example, jumping and lying on the ground at the same time is quite difficult...
//bool current_state_valid() const;

// Return value: <from_anim, to_anim, normalized_blend>
//std::tuple<noob::animated_model::animation_type,noob::animated_model::animation_type,float> get_current_anim() const;

// Sets the default at the most specific blend tree level possible
//bool set_default(const noob::animated_model::animation_type& anim);
//bool anim_exists(noob::animated_model::animation_type anim) const;
// If the desired animation isn't present, this will go to the most specific valid item in the blend tree.



