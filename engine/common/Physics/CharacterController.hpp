#pragma once
#include "PhysicsWorld.hpp"

namespace noob
{
	class character_controller
	{
		public:
		
			void init(const noob::physics_world& world, const std::shared_ptr<noob::physics_shape>&, const noob::mat4& position = noob::identity_mat4(), float max_speed = 10.0, float mass = 1.0, float step_height = 0.25);
			void pre_step();
			void player_step(float dt, bool forwards, bool backwards, bool left, bool right, bool jump);
			void jump();

			bool on_ground() const;
			noob::mat4 get_transform() const;
		//	noob::physics_body get_physics_body() const;

		protected:
			noob::physics_body body;
			//noob::physics_shape shape;
			//noob::physics_world& world;
			float turning_angle, max_linear_velocity, walk_velocity, turn_velocity;
	};
}
