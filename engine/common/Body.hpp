#pragma once


#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Shape.hpp"

namespace noob
{
	class body
	{
		public:
			struct info
			{
				void init(btRigidBody* _body)
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
				}

				float mass, friction, restitution;
				noob::vec3 linear_factor, angular_factor, position, linear_velocity, angular_velocity;
				noob::versor orientation;
			};

			template <class Archive>
				void serialize(Archive& ar)
				{
					noob::body::info info;
					info.init(inner_body);
					ar(info);
				}

			void init(btDynamicsWorld*, const noob::shape*, float mass, const noob::vec3& position, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			void init(btDynamicsWorld*, const noob::shape*, const noob::body::info&);
			
			// Common usefuls:
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;
			noob::vec3 get_linear_velocity() const;
			noob::vec3 get_angular_velocity() const;
			std::string get_debug_string() const;
			btRigidBody* get_raw_ptr() const;
			noob::body::info get_info() const;

			protected:
			
			btRigidBody* inner_body;
	};
}
