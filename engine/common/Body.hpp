// Kinematic, until self_controlled == false. The it beomes dynamic body
#pragma once

#include <btBulletDynamicsCommon.h>
#include <memory>

#include "MathFuncs.hpp"
#include "Shape.hpp"

namespace noob
{
	class stage;
	class joint;
	enum class body_type {DYNAMIC = 0, KINEMATIC = 1, STATIC = 2}; //, GHOST = 3};

	class body 
	{
		friend class stage;
		friend class joint;

		public:
			struct info
			{
				void init(btRigidBody* _body, noob::body_type _type, bool _ccd)
				{
					float inv_mass = _body->getInvMass();
					if (inv_mass > 0.0)
					{
						mass = 1.0 / inv_mass;
					}
					else mass = 0.0;
					type = _type;
					friction = _body->getFriction();
					restitution = _body->getRestitution();
					position = _body->getCenterOfMassPosition();
					orientation =  _body->getOrientation();
					linear_velocity = _body->getLinearVelocity();
					angular_velocity = _body->getAngularVelocity();
					linear_factor = _body->getLinearFactor();
					angular_factor = _body->getAngularFactor();
					type = _type;
					ccd = _ccd;
				}

				float mass, friction, restitution;
				noob::vec3 position, linear_velocity, angular_velocity, linear_factor, angular_factor;
				noob::versor orientation;
				bool ccd;
				noob::body_type type;
			};


			body() : physics_valid(false), is_physical(true) {}
			~body();

			void init(btDynamicsWorld*, noob::body_type, const noob::shape*, float mass, const noob::vec3& position, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);
			void init(btDynamicsWorld*, noob::body_type, const noob::shape*, const noob::body::info&);

			// void set_type(noob::body_type);
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);

			noob::vec3 get_position() const;
			noob::versor get_orientation() const;

			noob::mat4 get_transform() const;
			noob::vec3 get_linear_velocity() const;
			noob::vec3 get_angular_velocity() const;

			noob::body::info get_info() const;

			std::string get_debug_string() const;
			
			void set_ccd(bool); 
			
			// This is a hack to allow Bullet (or anyone else) to introspect whether this is a physical or ghost object. Useful when filtering collisions between bodies and ghosts.
			const bool is_physical;
		
		protected:
			noob::body_type type;
			bool ccd, physics_valid;
			btDynamicsWorld* dynamics_world;
			btRigidBody* inner_body;
	};
}
