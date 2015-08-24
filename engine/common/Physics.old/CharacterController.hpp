#pragma once
#include "PhysicsWorld.hpp"

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : self_control(true) {}	

			void init(const noob::physics_world&, const noob::mat4& xform, float _height, float _width, float _mass, float _max_speed); //, float step_height);

			void step(float dt, bool forward, bool backward, bool left, bool right, bool jump);
			//void jump();

			bool on_ground() const;
			noob::mat4 get_transform() const;
			noob::physics_body get_physics_body() const { return body; }
			
			void print_debug_info() const { body.print_debug_info(); }
		protected:
			
			void pre_step();
			// If true, this means that the character is still able to do things under its own control. IE: Not being thrown around by greater forces.
			bool self_control, airborne;
			noob::physics_body body;
			noob::vec3 target_position;
			float height, width, mass, max_speed, turn_speed, walk_speed;
			// Needs a reference to the physics world in order to raycast.
			noob::physics_world world;
	};
}
