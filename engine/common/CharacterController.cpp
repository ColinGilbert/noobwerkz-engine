#include "CharacterController.hpp"
#include "TransformHelper.hpp"
#include "Logger.hpp"


#include <cmath>

void noob::character_controller::init(btDynamicsWorld* _dynamics_world, btScalar _height, btScalar _width, btScalar _step_height, float _timestep)
{
	dynamics_world = _dynamics_world;
	height = _height;
	width = _width;
	half_height = height / btScalar(2.0);

	shape = new btCapsuleShape(width / 2.0, height);//new btMultiSphereShape(btVector3(width/btScalar(2.0), height/btScalar(2.0), width/btScalar(2.0)), &spherePositions[0], &sphereRadii[0], 2);

	btTransform start_transform;
	start_transform.setIdentity();
	start_transform.setOrigin(btVector3(0.0, 2.0, 0.0));

	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	btRigidBody::btRigidBodyConstructionInfo ci(1.0, motion_state, shape);
	rigid_body = new btRigidBody(ci);
	rigid_body->setSleepingThresholds(0.0, 0.0);
	rigid_body->setAngularFactor(0.0);
	dynamics_world->addRigidBody(rigid_body);
	dt = _timestep;
}


void noob::character_controller::destroy(btDynamicsWorld* dynamics_world)
{
	if (shape)
	{
		delete shape;
	}

	if (rigid_body)
	{
		dynamics_world->removeRigidBody(rigid_body);
		delete rigid_body;
	}
}


//btRigidBody* noob::character_controller::get_body()
//{
//	return rigid_body;
//}


void noob::character_controller::set_position(const noob::vec3& pos)
{
	btTransform t = rigid_body->getCenterOfMassTransform();
	t.setOrigin(btVector3(pos.v[0], pos.v[1], pos.v[2]));
	rigid_body->setCenterOfMassTransform(t);
}


void noob::character_controller::set_orientation(const noob::versor& orient)
{
	btTransform t = rigid_body->getCenterOfMassTransform();
	t.setRotation(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]));
	rigid_body->setCenterOfMassTransform(t);

}


void noob::character_controller::update() 
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);
	btVector3 down = -xform.getBasis()[1];
	btVector3 forward = xform.getBasis()[2];
	down.normalize();
	forward.normalize();

	ray_source[0] = xform.getOrigin();
	ray_source[1] = xform.getOrigin();


	ray_target[0] = ray_source[0] + btVector3(0.0, -(half_height+1.0), 0.0);
	ray_target[1] = ray_source[1] + btVector3(2.0, 0.0, 0.0);


	for (int i = 0; i < 2; ++i)
	{
		btCollisionWorld::ClosestRayResultCallback ray_callback(ray_source[i], ray_target[i]);

		dynamics_world->rayTest(ray_source[i], ray_target[i], ray_callback);
		if (ray_callback.hasHit())
		{
			if (i == 0) airborne = false;
			if (i == 1) obstacle = false;
			logger::log("[Character] - ray hit!");
			ray_lambda[i] = ray_callback.m_closestHitFraction;
		}
		else
		{
			if (i == 0) airborne = true;
			if (i == 1) obstacle = true;
			logger::log("[Character] - no hit :(");
			ray_lambda[i] = 1.0;
		}
	}
}

void noob::character_controller::step(bool forward, bool backward, bool left, bool right, bool jump)
{
if (on_ground())
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);

	btVector3 linear_velocity = rigid_body->getLinearVelocity();
	btScalar speed = rigid_body->getLinearVelocity().length();
	//btVector3 forward_dir = xform.getBasis()[2];
	//forward_dir.normalize();
	btVector3 walk_direction = btVector3(0.0, 0.0, 0.0);
	btScalar walk_speed = 2.0 * dt;

	
	btVector3 forward_dir(walk_speed, 0.0, 0.0);


	if (forward) walk_direction += forward_dir;
	if (backward) walk_direction -= forward_dir;

	if (!forward && !backward && !left && !right && !jump && on_ground())
	{
		// Dampen when on the ground and not being moved by the player
		linear_velocity *= btScalar(0.2);
		//rigid_body->setLinearVelocity(linear_velocity);
	}
	else
	{
		if (speed < max_linear_velocity)
		{
			linear_velocity = linear_velocity + walk_direction * walk_speed;
			//rigid_body->setLinearVelocity(velocity);
		}
	}
	if (jump)
	{
		linear_velocity = linear_velocity + btVector3(0.0, 6.0, 0.0);
	}

	rigid_body->setLinearVelocity(linear_velocity);
}
}
//rigid_body->getMotionState()->setWorldTransform(xform);
	//rigid_body->setCenterOfMassTransform(xform);

/*
	if (on_ground())
	{
	
		// Handle turning
		//if (left) turn_angle -= dt * turn_velocity;
		//if (right) turn_angle += dt * turn_velocity;

		//xform.setRotation(btQuaternion(btVector3(0.0, 1.0, 0.0), turn_angle));

		btVector3 linear_velocity = rigid_body->getLinearVelocity();
		btScalar speed = rigid_body->getLinearVelocity().length();

		btVector3 forward_dir = xform.getBasis()[2];
		forward_dir.normalize();
		btVector3 walk_direction = btVector3(0.0, 0.0, 0.0);
		btScalar walk_speed = walk_velocity * dt;

		if (forward) walk_direction += forward_dir;
		if (backward) walk_direction -= forward_dir;

		if (!forward && !backward && on_ground())
		{
			// Dampen when on the ground and not being moved by the player
			linear_velocity *= btScalar(0.2);
			rigid_body->setLinearVelocity(linear_velocity);
		}
		else
		{
			if (speed < max_linear_velocity)
			{
				btVector3 velocity = linear_velocity + walk_direction * walk_speed;
				rigid_body->setLinearVelocity(velocity);
			}
		}

		rigid_body->getMotionState()->setWorldTransform(xform);
		rigid_body->setCenterOfMassTransform(xform);
	}
	*/

/*
bool noob::character_controller::can_jump() const
{
	return on_ground();
}

void noob::character_controller::jump()
{
	if (!can_jump()) return;

	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform (xform);
	btVector3 up = xform.getBasis()[1];
	up.normalize();
	btScalar magnitude = (btScalar(1.0)/rigid_body->getInvMass()) * btScalar(8.0);
	rigid_body->applyCentralImpulse(up * magnitude);
}
*/


bool noob::character_controller::on_ground() const
{
	return (!(airborne && obstacle));
	//	return ray_lambda[0] < btScalar(1.0);
}

noob::vec3 noob::character_controller::get_position() const
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);
	return xform.getOrigin();
}


noob::versor noob::character_controller::get_orientation() const
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);
	return xform.getRotation();
}


noob::mat4 noob::character_controller::get_transform() const
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);
	noob::transform_helper t;
	t.translate(get_position());
	t.rotate(get_orientation());
	return t.get_matrix();
}

std::string noob::character_controller::get_debug_info() const
{
	fmt::MemoryWriter w;
	w << "[Character] position " << get_position().to_string() << ", orientation " << get_orientation().to_string() << " on ground? " << on_ground() << " ray lambda # 1 = " << ray_lambda[0] << " ray lambda # 2 = " << ray_lambda[1];
	return w.str();
}

















/*
   void noob::character_controller::init(btDynamicsWorld* _world, const std::shared_ptr<noob::prop>& _prop)
   {
   world = _world;
   prop = _prop;
   }


   void noob::character_controller::update()
   {
   noob::vec3 from(prop->get_position().v[0], prop->get_position().v[1], prop->get_position().v[2]);
   noob::vec3 to(from.v[0], from.v[1] - ((height+1)/2.0), from.v[2]);
   }


   void noob::character_controller::step(bool forward, bool back, bool left, bool right, bool jump)
   {

// TODO: Replace nested conditionals with something more succinct
*/
/*	

	float move_factor = 10.0;
	float jump_force = 20.0;
	if (self_control)
	{
// prop.body->setTransform(rp3d::Transform(prop.body->getTransform().getPosition(), rp3d::Quaternion(0.0, 0.0, 0.0, 1.0)));	
prop.body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
prop.body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
noob::vec3 current_vel(0.0, 0.0, 0.0);

if (forward)
{
logger::log("[Character] going forward");
noob::vec3 direction = noob::cross(noob::vec3(0.0, 0.0, -move_factor), slope);
current_vel += direction;
}

if (back)
{
logger::log("[Character] going backward");
noob::vec3 direction = noob::cross(noob::vec3(0.0, 0.0, move_factor), slope);
current_vel += direction;
}

if (left)
{
logger::log("[Character] going left");
noob::vec3 direction = noob::cross(noob::vec3(-move_factor, 0.0, 0.0), slope);
current_vel += direction;
}

if (right)
{
logger::log("[Character] going right");
noob::vec3 direction = noob::cross(noob::vec3(move_factor, 0.0, 0.0), slope);
current_vel += direction;
}

if (jump)
{
logger::log("[Character] jumping");
current_vel += noob::vec3(0.0, jump_force, 0.0);
}

prop.body->setLinearVelocity(rp3d::Vector3(current_vel.v[0], current_vel.v[1], current_vel.v[2]));
}
*/
/*
   }

   void noob::character_controller::stop()
   {
//	prop.body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));		
//	prop.body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
}
*/
