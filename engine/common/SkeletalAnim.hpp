// This class represents an animated character. Current only does one animation at a time, in order to learn the system.
// TODO: Complete!
#pragma once


#include <unordered_map>
#include <math.h>

#include <noob/math/math_funcs.hpp>

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

#include "Graphics.hpp"


namespace noob
{
	class skeletal_anim
	{
		public:
			skeletal_anim(): valid(false), current_time(0.0), total_runtime(1.0), allocator(ozz::memory::default_allocator()) {}
			
			~skeletal_anim();
			// Loads a runtime skeleton. Possibly convert to raw skeleton
			void init(const std::string& filename);
			// Loads a raw animation. You then create runtime animations via optimize()
			bool load_animation_file(const std::string& filename, const std::string& anim_name);
			// If name = "" all animations get processed. If all the tolerances == 0.0 it doesn't run an optimization pre-pass prior to creating runtime animations. 
			void optimize(float translation_tolerance = 0.0, float rotation_tolerance = 0.0, float scale_tolerance = 0.0, const std::string& name = "");

			void update(float dt);
			void reset_time(float t = 0.0);
			void set_total_runtime(float r);
			bool anim_exists(const std::string& name) const;
			bool switch_to_anim(const std::string& name);
			std::string get_current_anim() const;
			// std::weak_ptr<noob::skeletal_anim::sampler> get_sampler(const std::string& anim_name) const;
			// Gets bone matrices in model space
			
			std::vector<noob::mat4f> get_matrices() const;
			/// std::array<noob::vec3f,4> get_skeleton_bounds() const;

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
				friend class noob::skeletal_anim;

				public:
				void update(float dt);
				float weight, runtime;

				ozz::Range<ozz::math::SoaTransform> get_local_mats() const;
				void get_model_mats(ozz::Range<ozz::math::Float4x4>&);
				
				protected:
				sampler() : weight(1.0), cache(nullptr) {}
				noob::skeletal_anim::playback_controller controller;

				ozz::animation::Animation* animation;
				ozz::animation::Skeleton* skeleton;
				ozz::animation::SamplingCache* cache;
				ozz::Range<ozz::math::SoaTransform> locals;
			};
			
			
			noob::skeletal_anim::sampler create_sampler(const ozz::animation::Animation& anim);
			void destroy_sampler(noob::skeletal_anim::sampler&);		
			
			bool valid;
			std::string current_anim_name;
			ozz::animation::Animation* current_anim;
			float current_time, total_runtime;

			std::unordered_map<std::string, ozz::animation::Animation*> runtime_anims;
			std::unordered_map<std::string, ozz::animation::offline::RawAnimation> raw_anims;
			std::unordered_map<std::string, noob::skeletal_anim::sampler> samplers;

			// float threshold;
			// size_t num_layers;
			// lemon::StaticDigraph blend_tree;
			// ozz::Range<ozz::math::SoaTransform> blended_locals;
			
			ozz::animation::Skeleton skeleton;
			ozz::Range<ozz::math::Float4x4> model_matrices;
			ozz::memory::Allocator* allocator;
	};

	typedef noob::handle<noob::skeletal_anim> skeletal_anim_handle;
}
