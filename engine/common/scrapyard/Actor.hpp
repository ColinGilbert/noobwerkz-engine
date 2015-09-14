// The actor class holds a character controller and tells it where to go.

#pragma once

#include <vector>
#include <memory>
#include <deque>
#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "CharacterController.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"
#include "Drawable.hpp"

namespace noob
{
	class actor
	{
		public:
			actor() : anim_time(0.0) {}
			
			void init(btDynamicsWorld*, noob::drawable*, noob::skeletal_anim*);

			void update();
			void move(bool forward = false, bool backward = false, bool left = false, bool right = false, bool jump = false);
			void set_position(const noob::vec3& pos) { controller.set_position(pos); }
			void set_orientation(const noob::versor& orient) { controller.set_orientation(orient); }	
			void set_destination(const noob::vec3&);
			bool add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();

			noob::mat4 get_transform() const { return controller.get_transform(); }
			noob::vec3 get_position() const { return controller.get_position(); }
			noob::versor get_orientation() const { return controller.get_orientation(); }
			noob::vec3 get_destination() const;
			std::vector<noob::vec3> get_path() const;
					
			std::string get_debug_info() const { return controller.get_debug_info(); }

		protected:
			noob::character_controller controller;
			noob::drawable* drawable;
			std::deque<noob::vec3> path;
			noob::skeletal_anim* anim;
			std::string current_anim;
			float anim_time, max_speed;
			

	};
}
