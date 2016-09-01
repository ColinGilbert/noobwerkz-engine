#include "Body.hpp"

#include "Logger.hpp"
#include "Globals.hpp"


void noob::body::init(btDynamicsWorld* const dynamics_world, noob::body_type type_arg, const noob::shape& shape, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true) 
{
	btTransform start_transform;

	start_transform.setRotation(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]));
	start_transform.setOrigin(btVector3(pos.v[0], pos.v[1], pos.v[2]));

	float _mass = mass;

	type = type_arg;

	switch(type)
	{
		case(noob::body_type::DYNAMIC):
			{
				break;
			}
		case(noob::body_type::KINEMATIC):
			{
				_mass = 0.0;
				break;
			}
		case(noob::body_type::STATIC):
			{
				_mass = 0.0;
				break;
			}
		default:
			{
				break;
			}
	};


	btVector3 inertia(0.0, 0.0, 0.0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);

	if (type == noob::body_type::DYNAMIC)
	{
		shape.inner_shape->calculateLocalInertia(_mass, inertia);
	}

	btRigidBody::btRigidBodyConstructionInfo ci(_mass, motion_state, shape.inner_shape, inertia);
	inner = new btRigidBody(ci);

	set_ccd(ccd);

	dynamics_world->addRigidBody(inner);
	noob::globals& g = noob::globals::get_instance();
	physics_valid = true;	
}


void noob::body::init(btDynamicsWorld* const dynamics_world, noob::body_type type_arg, const noob::shape& shape, const noob::body::info& _info) noexcept(true) 
{
	btTransform start_transform;
	// start_transform.setIdentity();
	start_transform.setRotation(btQuaternion(_info.orientation.q[0], _info.orientation.q[1], _info.orientation.q[2], _info.orientation.q[3]));
	start_transform.setOrigin(btVector3(_info.position.v[0], _info.position.v[1], _info.position.v[2]));
	btVector3 inertia(0, 0, 0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	shape.inner_shape->calculateLocalInertia(_info.mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo ci(_info.mass, motion_state, shape.inner_shape, inertia);
	inner = new btRigidBody(ci);
	inner->setFriction(_info.friction);
	inner->setRestitution(_info.restitution);
	inner->setAngularFactor(btVector3(_info.angular_factor.v[0], _info.angular_factor.v[1], _info.angular_factor.v[2]));
	inner->setLinearFactor(btVector3(_info.linear_factor.v[0], _info.linear_factor.v[1], _info.linear_factor.v[2]));
	inner->setLinearVelocity(btVector3(_info.linear_velocity.v[0], _info.linear_velocity.v[1], _info.linear_velocity.v[2]));
	inner->setAngularVelocity(btVector3(_info.angular_velocity.v[0], _info.angular_velocity.v[1], _info.angular_velocity.v[2]));
	set_ccd(_info.ccd);
	dynamics_world->addRigidBody(inner);	
	physics_valid = true;
}

/*
   void noob::body::set_position(const noob::vec3& pos)
   {
   }


   void noob::body::set_orientation(const noob::versor& orient)
   {
   }

void noob::body::set_self_controlled(bool b) noexcept(true) 
{
	if (b == true)
	{
		self_controlled = true;
		// inner->setLinearSleepingThreshold(btScalar(0.8));
		// inner->setAngularSleepingThreshold(btScalar(1.0));
		// inner->setAngularFactor(btVector2(0.0, 0.0));
		// inner->setLinearFactor(btVector3(1.0, 1.0, 1.0));
	}
	else
	{
		self_controlled = false;
	}
}


void noob::body::update() 
{
	if (self_controlled)
	{
		btTransform xform;

		inner->getMotionState()->getWorldTransform(xform);

		btVector3 down = -xform.getBasis()[1];
		btVector3 forward = xform.getBasis()[2];

		down.normalize();
		forward.normalize();

		btVector3 ray_source = xform.getOrigin();
		btVector3 ray_target = ray_source + btVector3(0.0, -((height * 0.5) + 5.0), 0.0);

		btCollisionWorld::ClosestRayResultCallback ray_callback(ray_source, ray_target);

		dynamics_world->rayTest(ray_source, ray_target, ray_callback);

		if (ray_callback.hasHit())
		{
			airborne = false;
			// if (i == 0) airborne = false;
			// if (i == 1) obstacled = false;
			// logger::log("[Character] - ray hit!");
			ray_lambda = ray_callback.m_closestHitFraction;
		}
		else
		{
			airborne = true;
			// logger::log("[Character] - no hit");
			ray_lambda = 1.0;
		}
	}
}


void noob::body::move(bool forward, bool backward, bool left, bool right, bool jump) noexcept(true) 
{
	if (self_controlled)
	{
		if (on_ground())
		{
			btTransform xform;
			inner->getMotionState()->getWorldTransform(xform);

			btVector3 linear_velocity = inner->getLinearVelocity();
			btScalar speed = inner->getLinearVelocity().length();
			btVector3 walk_direction = btVector3(0.0, 0.0, 0.0);

			btVector3 forward_dir(walk_speed, 0.0, 0.0);

			if (forward) walk_direction += forward_dir;
			if (backward) walk_direction -= forward_dir;

			if (jump)
			{
				linear_velocity += btVector3(0.0, 1.5, 0.0);
			}
			if (!forward && !backward && !left && !right && !jump && on_ground())
			{
				// Dampen when on the ground and not being moved by the player
				linear_velocity *= btScalar(0.2);
			}
			else
			{
				if (speed < max_linear_velocity)
				{
					linear_velocity = linear_velocity + walk_direction * walk_speed;
				}
			}
		}
	}
}


bool noob::body::on_ground() const noexcept(true) 
{
	return (!(airborne && obstacled));
	//	return ray_lambda[0] < btScalar(1.0);
}
*/


noob::vec3 noob::body::get_position() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return vec3_from_bullet(xform.getOrigin());
}


noob::versor noob::body::get_orientation() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return versor_from_bullet(xform.getRotation());
}


noob::vec3 noob::body::get_linear_velocity() const noexcept(true) 
{
	return vec3_from_bullet(inner->getLinearVelocity());
}


noob::vec3 noob::body::get_angular_velocity() const noexcept(true) 
{
	return vec3_from_bullet(inner->getAngularVelocity());
}



noob::mat4 noob::body::get_transform() const noexcept(true) 
{
	btTransform xform;
	inner->getMotionState()->getWorldTransform(xform);
	return mat4_from_bullet(xform);
}


std::string noob::body::get_debug_string() const noexcept(true) 
{
	return noob::concat("[Body] position ", noob::to_string(get_position()), ", orientation ", noob::to_string(get_orientation()), ", linear velocity ", noob::to_string(get_linear_velocity()), ", angular velocity ", noob::to_string(get_angular_velocity()));// << ", on ground? " << on_ground() << ", ray lambda  = " << ray_lambda; //<< " ray lambda # 2 = " << ray_lambda[1];
}

void noob::body::set_ccd(bool b) noexcept(true) 
{
	if (b == true)
	{
		btVector3 center;
		btScalar radius;
		inner->getCollisionShape()->getBoundingSphere(center, radius);
		inner->setCcdMotionThreshold(radius);
	}
}



