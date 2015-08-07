#pragma once

#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "AnimatedModel.hpp"
#include "PhysicsWorld.hpp"
//#include "ActorFactory.hpp"

namespace noob
{
	class actor
	{
		//friend class noob::actor_factory;
		
		public:
			void init(const noob::physics_body& bod);
			void set_model(const std::shared_ptr<noob::animated_model>& model);
			void set_position(const noob::vec3& pos);
			void set_orientation(const noob::versor& orient);
			void add_to_path(const std::vector<noob::vec3>& path_segment, float speed);
			void clear_path();
			void face_point(const noob::vec3& point);
			void stop_moving();
			void start_moving();


		protected:

			bool moving;
			float min_speed, max_speed, jump_height;
			noob::physics_body actor_body, destination;
			std::shared_ptr<noob::animated_model> model;
			std::deque<std::tuple<std::vector<noob::vec3>, float>> path;
	};
}
