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
			void init(const std::shared_ptr<noob::physics_body>&, const std::shared_ptr<noob::drawable3d>&, const std::shared_ptr<noob::animated_model>&);


			void set_position(const noob::vec3&);
			noob::vec3 get_position() const;
			void set_orientation(const noob::versor&);
			noob::versor get_orientation() const;
			void set_transform(const noob::mat4&);
			noob::mat4 get_transform() const;
			
			void add_to_path(const std::vector<noob::vec3>& path_segment);
			std::vector<noob::vec3> get_path() const;
			void clear_path();
			
			void face_point(const noob::vec3& point);
			
			void stop();
			void start();
			void draw_skeleton();
			void manual_control(bool);

		protected:
			std::string drawable_name;
			std::string animated_model_name;
			bool moving;
			float min_speed, max_speed, jump_height;
			noob::physics_body actor_body, destination;
			std::shared_ptr<noob::animated_model> model;
			std::deque<noob::vec3> path;
	};
}
