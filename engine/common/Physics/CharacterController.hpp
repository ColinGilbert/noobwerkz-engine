#pragma once


namespace noob
{
	class character_controller
	{
		public:
			void init(const noob::physics_world& world, float height = 2.0, float width = 0.25, float step_height = 0.25);
			void destroy();
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
