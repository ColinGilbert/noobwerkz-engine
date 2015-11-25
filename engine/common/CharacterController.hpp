// Kinematic, until self_control == false. The it beomes dynamic body
#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>

#include "Body.hpp"
#include "MathFuncs.hpp"


namespace noob
{

	class character_controller 
	{
		public:
			character_controller() : half_height(0.0), shape(nullptr), rigid_body(nullptr), dynamics_world(nullptr), ray_lambda(1.0), airborne(true), obstacle(true), turn_angle(1.0), dt(1.0/60.0), max_linear_velocity(10.0), walk_velocity(8.0), turn_velocity(1.0) {}

			void init(btDynamicsWorld*, noob::body*, float _timestep = 1.0 / 60.0);


			void move(bool forward, bool backward, bool left, bool right, bool jump);
			void jump();

			bool can_jump() const;
			bool on_ground() const;
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);
			
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;
			
			std::string get_debug_string() const;

		protected:
			void update();

			btScalar height;
			btScalar half_height;
			btScalar width;
			btCollisionShape* shape;
			
			noob::body* rigid_body;
			btDynamicsWorld* dynamics_world;

			btVector3 ray_source;
			btVector3 ray_target;
			btScalar ray_lambda;
			btVector3 ray_normal;

			bool airborne, obstacle;
			btScalar step_height;
			btScalar turn_angle;
			btScalar dt;
			btScalar max_linear_velocity;
			btScalar walk_velocity;
			btScalar turn_velocity;
		
	};
}
































/*
   namespace noob
   {
   class character_controller
   {
   public:
   character_controller() : resting(false), self_control(false), time_on_ground(0.0), slope(0.0, 0.0, 0.0), world(nullptr) {}

   void init(btDynamicsWorld*, const std::shared_ptr<noob::prop>&);
   void update();
   void move(bool forward, bool back, bool left, bool right, bool jump);
   void stop();
   noob::mat4 get_transform() const { return prop->get_transform(); }
   noob::vec3 get_position() const { return prop->get_position(); }
   noob::versor get_orientation() const { return prop->get_orientation(); }
   std::string get_debug_info() const { return prop->get_debug_info(); }
   noob::prop* get_prop() const { return prop.get(); }

   protected:
   bool resting, self_control;
   float width, mass, height, current_speed, max_speed, time_on_ground;
   noob::vec3 slope;
   std::shared_ptr<noob::prop> prop;
   btDynamicsWorld* world;
   };
   }
   */
