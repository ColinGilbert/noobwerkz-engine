#include "Body.hpp"
#include "TransformHelper.hpp"
#include "Logger.hpp"


#include <cmath>

noob::body::~body()
{
	if (physics_valid)
	{
		// dynamics_world->removeRigidBody(inner_body);
		// delete inner_body;
	}
}




void noob::body::init(NewtonWorld* _dynamics_world, noob::body_type type_arg, const noob::shape* _shape, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd)
{
	dynamics_world = _dynamics_world;
	type = type_arg;
	// btTransform start_transform;
	// start_transform.setIdentity();
	// start_transform.setOrigin(btVector3(pos.v[0], pos.v[1], pos.v[2]));
	// start_transform.setRotation(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]));

	switch(type)
	{
		case(noob::body_type::DYNAMIC):
		{
			break;
		}
		case(noob::body_type::KINEMATIC):
		{
			break;
		}
		case(noob::body_type::STATIC):
		{
			break;
		}
		case(noob::body_type::GHOST):
		{
			break;
		}

	};


	// btVector3 inertia(0.0, 0.0, 0.0);
	// btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	shape = const_cast<noob::shape*>(_shape);
	// shape->inner_shape->calculateLocalInertia(mass, inertia);
	
	// btRigidBody::btRigidBodyConstructionInfo ci(mass, motion_state, shape->inner_shape, inertia);
	// inner_body = new btRigidBody(ci);
	set_ccd(ccd);
	// dynamics_world->addRigidBody(inner_body);
	// physics_valid = true;
}

void noob::body::init(NewtonWorld* _dynamics_world, noob::body_type type_arg, const noob::shape* _shape, const noob::body::info& _info)
{
	dynamics_world = _dynamics_world;
	// btTransform start_transform;
	// start_transform.setIdentity();
	// start_transform.setOrigin(btVector3(_info.position.v[0], _info.position.v[1], _info.position.v[2]));
	// start_transform.setRotation(btQuaternion(_info.orientation.q[0], _info.orientation.q[1], _info.orientation.q[2], _info.orientation.q[3]));
	// btVector3 inertia(0, 0, 0);
	// btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	// shape = const_cast<noob::shape*>(_shape);
	// shape->inner_shape->calculateLocalInertia(_info.mass, inertia);
	// btRigidBody::btRigidBodyConstructionInfo ci(_info.mass, motion_state, shape->inner_shape, inertia);
	// inner_body = new btRigidBody(ci);
	// inner_body->setFriction(_info.friction);
	// inner_body->setRestitution(_info.restitution);
	// inner_body->setAngularFactor(btVector3(_info.angular_factor.v[0], _info.angular_factor.v[1], _info.angular_factor.v[2]));
	// inner_body->setLinearFactor(btVector3(_info.linear_factor.v[0], _info.linear_factor.v[1], _info.linear_factor.v[2]));
	// inner_body->setLinearVelocity(btVector3(_info.linear_velocity.v[0], _info.linear_velocity.v[1], _info.linear_velocity.v[2]));
	// inner_body->setAngularVelocity(btVector3(_info.angular_velocity.v[0], _info.angular_velocity.v[1], _info.angular_velocity.v[2]));
	set_ccd(_info.ccd);
	// dynamics_world->addRigidBody(inner_body);

	physics_valid = true;
}


void noob::body::set_type(noob::body_type type_arg)
{
	if (type != type_arg)
	{
		switch (type_arg)
		{
			case(noob::body_type::DYNAMIC):
				
				break;
			case(noob::body_type::KINEMATIC):
				
				break;
			case(noob::body_type::STATIC):
				
				break;
			case(noob::body_type::GHOST):
				
				break;
		};
	}
	
	type = type_arg;

}


/*
   void noob::body::set_position(const noob::vec3& pos)
   {
   }


   void noob::body::set_orientation(const noob::versor& orient)
   {
   }
   */
/*
void noob::body::set_self_controlled(bool b)
{
	if (b == true)
	{
		self_controlled = true;
		// inner_body->setLinearSleepingThreshold(btScalar(0.8));
		// inner_body->setAngularSleepingThreshold(btScalar(1.0));
		// inner_body->setAngularFactor(btVector2(0.0, 0.0));
		// inner_body->setLinearFactor(btVector3(1.0, 1.0, 1.0));
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

		inner_body->getMotionState()->getWorldTransform(xform);

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


void noob::body::move(bool forward, bool backward, bool left, bool right, bool jump)
{
	if (self_controlled)
	{
		if (on_ground())
		{
			btTransform xform;
			inner_body->getMotionState()->getWorldTransform(xform);

			btVector3 linear_velocity = inner_body->getLinearVelocity();
			btScalar speed = inner_body->getLinearVelocity().length();
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


bool noob::body::on_ground() const
{
	return (!(airborne && obstacled));
	//	return ray_lambda[0] < btScalar(1.0);
}
*/


noob::vec3 noob::body::get_position() const
{
	// btTransform xform;
	// inner_body->getMotionState()->getWorldTransform(xform);
	// return xform.getOrigin();
}


noob::versor noob::body::get_orientation() const
{
	// btTransform xform;
	// inner_body->getMotionState()->getWorldTransform(xform);
	// return xform.getRotation();
}


noob::vec3 noob::body::get_linear_velocity() const
{
	// return inner_body->getLinearVelocity();
}


noob::vec3 noob::body::get_angular_velocity() const
{
	// return inner_body->getAngularVelocity();	
}



noob::mat4 noob::body::get_transform() const
{
	// btTransform xform;
	// inner_body->getMotionState()->getWorldTransform(xform);
	// noob::transform_helper t;
	// t.translate(get_position());
	// t.rotate(get_orientation());
	// return t.get_matrix();
}


std::string noob::body::get_debug_string() const
{
	fmt::MemoryWriter w;
	w << "[Body] position " << get_position().to_string() << ", orientation " << get_orientation().to_string() << ", linear velocity " << get_linear_velocity().to_string() << ", angular velocity " << get_angular_velocity().to_string();// << ", on ground? " << on_ground() << ", ray lambda  = " << ray_lambda; //<< " ray lambda # 2 = " << ray_lambda[1];
	return w.str();
}

void noob::body::set_ccd(bool b)
{
	if (b == true)
	{
		// btVector3 center;
		// btScalar radius;
		// shape->inner_shape->getBoundingSphere(center, radius);
		// inner_body->setCcdMotionThreshold(radius);
	}
}
