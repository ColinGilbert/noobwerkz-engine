#pragma once

#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Shape.hpp"


namespace noob
{
	class stage;
	class joint;

	enum class body_type {DYNAMIC = 0, KINEMATIC = 1, STATIC = 2};

	class body 
	{
		friend class stage;
		friend class joint;

		public:	
		body() noexcept(true) : physics_valid(false), ccd(false) {}

		struct info
		{
			void init(btRigidBody* _body, noob::body_type _type, bool _ccd) noexcept(true)
			{
				float inv_mass = _body->getInvMass();
				if (inv_mass > 0.0)
				{
					mass = 1.0 / inv_mass;
				}
				else
				{
					mass = 0.0;
				}
				type = _type;
				friction = _body->getFriction();
				restitution = _body->getRestitution();
				position = vec3_from_bullet(_body->getCenterOfMassPosition());
				linear_velocity = vec3_from_bullet(_body->getLinearVelocity());
				angular_velocity = vec3_from_bullet(_body->getAngularVelocity());
				linear_factor = vec3_from_bullet(_body->getLinearFactor());
				angular_factor = vec3_from_bullet(_body->getAngularFactor());
				orientation =  versor_from_bullet(_body->getOrientation());
				ccd = _ccd;
			}

			noob::body_type type;
			float mass, friction, restitution;
			noob::vec3 position, linear_velocity, angular_velocity, linear_factor, angular_factor;
			noob::versor orientation;
			bool ccd;
		};

		void init(btDynamicsWorld* const, noob::body_type, const noob::shape&, float mass, const noob::vec3& position, const noob::versor& orientation, bool ccd) noexcept(true);
		void init(btDynamicsWorld* const, noob::body_type, const noob::shape&, const noob::body::info&) noexcept(true);

		// void set_type(noob::body_type) noexcept(true);

		void set_position(const noob::vec3&) noexcept(true);
		void set_orientation(const noob::versor&) noexcept(true);

		noob::vec3 get_position() const noexcept(true);
		noob::versor get_orientation() const noexcept(true);

		noob::mat4 get_transform() const noexcept(true);
		noob::vec3 get_linear_velocity() const noexcept(true);
		noob::vec3 get_angular_velocity() const noexcept(true);

		noob::body::info get_info() const noexcept(true);

		std::string get_debug_string() const noexcept(true);

		void set_ccd(bool) noexcept(true); 


		static uint32_t get_shape_index(const noob::body& b) noexcept(true)
		{
			return b.inner->getCollisionShape()->getUserIndex();
		}


		protected:
		noob::body_type type;
		bool physics_valid, ccd;
		btRigidBody* inner;
	};
}
