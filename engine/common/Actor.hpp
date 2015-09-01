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
			
			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::prop>&, const std::shared_ptr<noob::skeletal_anim>&, float width, float height);
			
			void set_skeleton(const std::shared_ptr<noob::skeletal_anim>& _anim);
			void update();
			void move(bool forward = false, bool backward = false, bool left = false, bool right = false, bool jump = false);
			
			void set_destination(const noob::vec3&);
			noob::vec3 get_destination() const;

			noob::mat4 get_transform() const;
			noob::vec3 get_position() const { return controller.get_position(); }
			noob::versor get_orientation() const { return controller.get_orientation(); }

			bool add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;

			std::string get_debug_info() const { return controller.get_debug_info(); }
			noob::prop* get_prop() const { return controller.get_prop(); }
			
		
		protected:
			noob::character_controller controller;
			std::shared_ptr<noob::skeletal_anim> anim;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time, max_speed;
			

	};
}
