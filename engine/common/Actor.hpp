#pragma once

#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "AnimatedModel.hpp"
#include "PhysicsWorld.hpp"
//#include "ActorFactory.hpp"
#include "Drawable3D.hpp"

namespace noob
{
	class actor
	{
		//friend class noob::actor_factory;
		public:
			actor() : physics_driven(true) {}
			// void set_position(const noob::vec3&);
			// noob::vec3 get_position() const;
			// void set_orientation(const noob::versor&);
			//noob::versor get_orientation() const;
			void set_physics_control(bool);
			void set_transform(const noob::mat4&);
			noob::mat4 get_transform();
			
			void add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			void face_point(const noob::vec3& point);
			
			void draw_skeleton() const;
			std::vector<noob::vec3> get_path() const;

		protected:
			float min_speed, max_speed, jump_height;
			noob::physics_body body, destination;
			std::shared_ptr<noob::animated_model> model;
			std::shared_ptr<noob::drawable3d> drawable;
			std::deque<noob::vec3> path;
			noob::mat4 xform;
			bool physics_driven;
	};
}
