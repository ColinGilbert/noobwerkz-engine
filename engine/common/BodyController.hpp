// Kinematic, until self_controlled == false. The it beomes dynamic body
#pragma once

#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

#include <btBulletDynamicsCommon.h>
#include <memory>

#include "Body.hpp"
#include "MathFuncs.hpp"
#include "Shape.hpp"

namespace noob
{
	class body_controller 
	{
		friend class stage;
		public:
			struct info
			{
				void init(btRigidBody* _body, bool _self_controlled, bool _ccd)
				{
					float inv_mass = _body->getInvMass();
					if (inv_mass > 0.0)
					{
						mass = 1.0 / inv_mass;
					}
					else mass = 0.0;
					friction = _body->getFriction();
					restitution = _body->getRestitution();
					linear_factor = _body->getLinearFactor();
					angular_factor = _body->getAngularFactor();
					position = _body->getCenterOfMassPosition();
					orientation =  _body->getOrientation();
					linear_velocity = _body->getLinearVelocity();
					angular_velocity = _body->getAngularVelocity();
					self_controlled = _self_controlled;
					ccd = _ccd;
				}

				float mass, friction, restitution;
				noob::vec3 linear_factor, angular_factor, position, linear_velocity, angular_velocity;
				noob::versor orientation;
				bool ccd, self_controlled;

			};

			template <class Archive>
				void serialize(Archive& ar)
				{
					noob::body_controller::info info;
					info.init(inner_body, self_controlled, ccd);
					ar(info);
				}

			body_controller() : height(0.0), width(0.0), step_height(0.1), ray_lambda(1.0), turn_angle(1.0), dt(1.0/60.0), max_linear_velocity(10.0), walk_speed(0.5), turn_speed(0.5), jump_force(1.5), airborne(true), obstacled(false), self_controlled(false) {}


			void init(btDynamicsWorld*, const noob::shape*, float mass, const noob::vec3& position, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);
			void init(btDynamicsWorld*, const noob::shape*, const noob::body_controller::info&);

			void set_self_controlled(bool b);
			void set_walk_speed(float s);
			void set_jump_force(float j);
			
			void move(bool forward, bool backward, bool left, bool right, bool jump);
			void jump();

			bool can_jump() const;
			bool on_ground() const;
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);

			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;
			noob::vec3 get_linear_velocity() const;
			noob::vec3 get_angular_velocity() const;

			noob::body_controller::info get_info() const;

			std::string get_debug_string() const;

			float height, width, step_height, ray_lambda, turn_angle, dt, max_linear_velocity, walk_speed, turn_speed, jump_force; 


		protected:
			void set_ccd(bool); // Helper to set ccd, for write-once-only purposes.
			bool ccd;
			void update();
			bool airborne, obstacled, self_controlled;
			noob::shape* shape;
			// btCollisionShape* inner_shape;			
			btDynamicsWorld* dynamics_world;
			btRigidBody* inner_body;
	};
}
































/*
   namespace noob
   {
   class body_controller
   {
   public:
   body_controller() : resting(false), self_controlled(false), time_on_ground(0.0), slope(0.0, 0.0, 0.0), world(nullptr) {}

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
   bool resting, self_controlled;
   float width, mass, height, current_speed, max_speed, time_on_ground;
   noob::vec3 slope;
   std::shared_ptr<noob::prop> prop;
   btDynamicsWorld* world;
   };
   }
   */
