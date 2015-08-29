// The actor class holds a character controller and tells it where to go.

#pragma once

#include "Stage.hpp"
#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "CharacterController.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"
#include "Prop.hpp"

#include "reactphysics3d.h"


namespace noob
{
	class actor
	{
		friend class stage;
		public:
			actor() : anim_time(0.0) {}
			
			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::skeletal_anim>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& = noob::identity_mat4(), float _mass = 10.0, float _width = 1.0, float _height = 1.0, float _max_speed = 10.0);
			
			void set_skeleton(const std::shared_ptr<noob::skeletal_anim>& _anim);
			
			void update(double dt, bool forward = false, bool backward = false, bool left = false, bool right = false, bool jump = false);
			
			bool set_destination(const noob::vec3&);
			
			noob::mat4 get_transform() const;
			//noob::prop get_destination() const;

			bool add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;

			void print_debug_info() const { controller.get_prop()->print_debug_info(); }
			noob::prop* get_prop() const { return controller.get_prop(); }
			noob::prop::info get_destination() const { return destination_prop.get_info(); }
		
		protected:
			noob::prop destination_prop;
			rp3d::DynamicsWorld* world;
			noob::character_controller controller;
			std::shared_ptr<noob::skeletal_anim> anim;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time, max_speed;
	};
}
