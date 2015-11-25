#include "CharacterController.hpp"
#include "TransformHelper.hpp"
#include "Logger.hpp"


#include <cmath>

void noob::character_controller::init(btDynamicsWorld* _dynamics_world, noob::body* _rigid_body, float _timestep)
{
	dynamics_world = _dynamics_world;
	rigid_body = _rigid_body;
	rigid_body->get_raw_ptr()->setSleepingThresholds(0.0, 0.0);
	rigid_body->get_raw_ptr()->setAngularFactor(0.0);
	dt = _timestep;
}


void noob::character_controller::set_position(const noob::vec3& pos)
{
	rigid_body->set_position(pos);
}


void noob::character_controller::set_orientation(const noob::versor& orient)
{
	rigid_body->set_orientation(orient);
}


void noob::character_controller::update() 
{
	btTransform xform;
	rigid_body->get_raw_ptr()->getMotionState()->getWorldTransform(xform);
	btVector3 down = -xform.getBasis()[1];
	btVector3 forward = xform.getBasis()[2];
	down.normalize();
	forward.normalize();

	ray_source = xform.getOrigin();


	ray_target = ray_source + btVector3(0.0, -(half_height+5.0), 0.0);


	btCollisionWorld::ClosestRayResultCallback ray_callback(ray_source, ray_target);

	dynamics_world->rayTest(ray_source, ray_target, ray_callback);
	if (ray_callback.hasHit())
	{
		airborne = false;
		// if (i == 0) airborne = false;
		// if (i == 1) obstacle = false;
		//logger::log("[Character] - ray hit!");
		ray_lambda = ray_callback.m_closestHitFraction;
	}
	else
	{
		airborne = true;
		//logger::log("[Character] - no hit");
		ray_lambda = 1.0;
	}
}


void noob::character_controller::move(bool forward, bool backward, bool left, bool right, bool jump)
{
	if (on_ground())
	{
		btTransform xform;
		rigid_body->get_raw_ptr()->getMotionState()->getWorldTransform(xform);

		btVector3 linear_velocity = rigid_body->get_raw_ptr()->getLinearVelocity();
		btScalar speed = rigid_body->get_raw_ptr()->getLinearVelocity().length();
		btVector3 walk_direction = btVector3(0.0, 0.0, 0.0);
		
		btScalar walk_speed = 0.5;// / 3.0;
		
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
		rigid_body->get_raw_ptr()->setLinearVelocity(linear_velocity);
	}
}


bool noob::character_controller::on_ground() const
{
	return (!(airborne && obstacle));
	//	return ray_lambda[0] < btScalar(1.0);
}


noob::vec3 noob::character_controller::get_position() const
{
//	btTransform xform;
//	rigid_body->get_raw_ptr()->getMotionState()->getWorldTransform(xform);
//	return xform.getOrigin();
	return rigid_body->get_position();
}


noob::versor noob::character_controller::get_orientation() const
{
//	btTransform xform;
//	rigid_body->get_raw_ptr()->getMotionState()->getWorldTransform(xform);
//	return xform.getRotation();
	return rigid_body->get_orientation();
}


noob::mat4 noob::character_controller::get_transform() const
{
/*
	btTransform xform;
	rigid_body->get_raw_ptr()->getMotionState()->getWorldTransform(xform);
	noob::transform_helper t;
	t.translate(get_position());
	t.rotate(get_orientation());
	return t.get_matrix();
*/
return rigid_body->get_transform();
}


std::string noob::character_controller::get_debug_string() const
{
	fmt::MemoryWriter w;
	w << "[Character] position " << get_position().to_string() << ", orientation " << get_orientation().to_string() << " on ground? " << on_ground() << " ray lambda  = " << ray_lambda; //<< " ray lambda # 2 = " << ray_lambda[1];
	return w.str();
}
