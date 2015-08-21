#pragma once
#include "PhysicsWorld.hpp"

namespace noob
{
	class character_controller
	{
		public:
			character_controller(const noob::physics_world& world, const noob::physics_shape&, float mass = 1.0, float step_height = 0.25, const noob::mat4& position = noob::identity_mat4());
			~character_controller();

			// void init(const noob::physics_world& world, float mass = 1.0, float height = 1.0, float width = 0.25, float step_height = 0.25, const noob::mat4& position = noob::identity_mat4());
			// void destroy();
			void pre_step();
			void player_step(float dt, bool forwards, bool backwards, bool left, bool right, bool jump);
			void jump();

			bool on_ground() const;
			noob::physics_body& get_physics_body() const;

		protected:
			noob::physics_body body;
			noob::physics_shape shape;
			//noob::physics_world& world;
			float turning_angle, max_linear_velocity, walk_velocity, turn_velocity;
	};
}
