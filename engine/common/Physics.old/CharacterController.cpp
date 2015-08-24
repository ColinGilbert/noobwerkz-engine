#include "CharacterController.hpp"



void noob::character_controller::init(const noob::physics_world& _world, const noob::mat4& xform, float _height, float _width, float _mass, float _max_speed)
{
	world = _world;
	width = _width;
	height = _height;
	mass = _mass;
	max_speed = _max_speed;
	walk_speed = max_speed / 2;
	turn_speed = max_speed / 4;
	body.init(xform, world.capsule(width, height), mass);
	// body.set_angular_factor(noob::vec3(0.0, 0.0, 0.0));
	world.add(body, noob::physics_world::collision_type::CHARACTER, noob::physics_world::collision_type::CHARACTER | noob::physics_world::collision_type::TERRAIN);
}



void noob::character_controller::pre_step()
{
/*
	noob::vec3 pos(body.get_raw_ptr()->getWorldTransform().getOrigin().x(), body.get_raw_ptr()->getWorldTransform().getOrigin().z(), body.get_raw_ptr()->getWorldTransform().getOrigin().y());
	noob::vec3 springforce(0.0, 0.0, 0.0);
	btVector3 from(pos.v[0], pos.v[1], pos.v[2]);
	btVector3 to(pos.v[0], pos.v[1] - 10, pos.v[2]);
	btCollisionWorld::AllHitsRayResultCallback res(from, to);
	world->get_raw_ptr()->rayTest(from, to, res);

	airborne = true;
	for (int i = 0; i < res.m_hitPointWorld.size(); ++i)
	{
		if (res.m_collisionObjects[i] != body.get_raw_ptr())
		{
			btVector3 a = res.m_hitPointWorld[i];
			noob::vec3 hp(a.x(), a.z(), a.y());

			float d = std::sqrt(noob::get_squared_dist(pos, hp));
			float ideal_distance = 0.5 + height / 2;

			logger::log(fmt::format("[DynamicController] distance from ground = {0}, ideal distance = {1}", d, ideal_distance));

			if (d < ideal_distance)
			{
				float stiffness = 8000.0f;
				springforce = noob::vec3(0.0, 0.0, stiffness * std::max(1.9f - d, -0.05f));
				airborne = false;
				break;
			}
		}
	}

	body.get_raw_ptr()->applyCentralForce(btVector3(springforce.v[0], springforce.v[1], springforce.v[2]));
*/
}


void noob::character_controller::step(float dt, bool forward, bool backward, bool left, bool right, bool jump)
{
/*
	if (self_control)
	{
		pre_step();

		btTransform xform;
		body.get_raw_ptr()->getMotionState()->getWorldTransform(xform);

		float turn_angle = 0.0;

		// Handle turning
		if (left)
		{
			turn_angle -= dt * turn_speed;
		}
		if (right)
		{
			turn_angle += dt * turn_speed;
		}

		xform.setRotation(btQuaternion(btVector3(0.0, 1.0, 0.0), turn_angle));

		btVector3 linear_velocity = body.get_raw_ptr()->getLinearVelocity();
		btScalar speed = linear_velocity.length();

		btVector3 forward_dir = xform.getBasis()[2];

		forward_dir.normalize();
		btVector3 walk_direction = btVector3(0.0, 0.0, 0.0);
		btScalar walk_speed = walk_speed * dt;

		if (forward)
		{
			walk_direction += forward_dir;
		}
		else if (backward)
		{
			walk_direction -= forward_dir;
		}


		if (!forward && !backward && on_ground())
		{
			// Dampen when on the ground and not being moved by the player
			linear_velocity *= btScalar(0.2);
			body.get_raw_ptr()->setLinearVelocity(linear_velocity);
		}
		else
		{
			if (speed < max_speed)
			{
				btVector3 velocity = linear_velocity + walk_direction * walk_speed;
				body.get_raw_ptr()->setLinearVelocity(velocity);
			}
		}

		body.get_raw_ptr()->getMotionState()->setWorldTransform(xform);
		body.get_raw_ptr()->setCenterOfMassTransform(xform);
	}
*/
}

/*
void noob::character_controller::jump()
{
	if (!on_ground())
	{
		return;
	}


}
*/

bool noob::character_controller::on_ground() const
{
	return !airborne;
}


noob::mat4 noob::character_controller::get_transform() const
{
	return body.get_transform();
}

/*
   noob::physics_body noob::character_controller::get_physics_body() const
   {
   return body;
   }
   */

/*
#include "CharacterController.hpp"
#include "ClosestNotMeRaycastCallback.hpp"

noob::character_controller::character_controller()
{
ray_lambda[0] = 1.0;
ray_lambda[1] = 1.0;
half_height = 1.0;
turn_angle = 0.0;
max_linear_velocity = 10.0;
walk_speed = 8.0; // meters/sec
turn_speed = 1.0; // radians/sec
shape = nullptr;
rigid_body = nullptr;
}


void noob::character_controller::setup(const noob::physics_world& physics_world, btScalar height, btScalar width, btScalar stepHeight)
{
world = physics_world.get_raw_ptr();
btVector3 spherePositions[2];
btScalar sphereRadii[2];

sphereRadii[0] = width;
sphereRadii[1] = width;
spherePositions[0] = btVector3(0.0, (height/btScalar(2.0) - width), 0.0);
spherePositions[1] = btVector3(0.0, (-height/btScalar(2.0) + width), 0.0);

half_height = height/btScalar(2.0);

//	shape = new btMultiSphereShape(btVector3(width/btScalar(2.0), height/btScalar(2.0), width/btScalar(2.0)), &spherePositions[0], &sphereRadii[0], 2);

btTransform startTransform;
startTransform.setIdentity();
startTransform.setOrigin(btVector3(0.0, 2.0, 0.0));
btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
btRigidBody::btRigidBodyConstructionInfo cInfo(1.0, myMotionState, shape);
rigid_body = new btRigidBody(cInfo);
// kinematic vs. static doesn't work
//rigid_body->setCollisionFlags( rigid_body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
rigid_body->setSleepingThresholds(0.0, 0.0);
rigid_body->setAngularFactor(0.0);
world->addRigidBody(rigid_body);
}


void noob::character_controller::destroy()
{
if (shape)
{
delete shape;
}

if (rigid_body)
{
world->removeRigidBody(rigid_body);
delete rigid_body;
}
}


btCollisionObject* noob::character_controller::getCollisionObject()
{
return rigid_body;
}


void noob::character_controller::prestep()
{
btTransform xform;
rigid_body->getMotionState()->getWorldTransform (xform);
btVector3 down = -xform.getBasis()[1];
btVector3 forward = xform.getBasis()[2];
down.normalize ();
forward.normalize();

ray_source[0] = xform.getOrigin();
ray_source[1] = xform.getOrigin();

ray_target[0] = ray_source[0] + down * half_height * btScalar(1.1);
ray_target[1] = ray_source[1] + forward * half_height * btScalar(1.1);

btClosestNotMeRayResultCallback rayCallback(rigid_body);

int i = 0;
for (i = 0; i < 2; i++)
{
	rayCallback.m_closestHitFraction = 1.0;
	world->rayTest(ray_source[i], ray_target[i], rayCallback);
	if (rayCallback.HasHit())
	{
		ray_lambda[i] = rayCallback.m_closestHitFraction;
	} else {
		ray_lambda[i] = 1.0;
	}
}
}


void noob::character_controller::player_step(btScalar dt, bool forward, bool backward, bool left, bool right, bool jump)
{
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);

	// Handle turning
	if (left)
	{
		turn_angle -= dt * turn_speed;
	}
	if (right)
	{
		turn_angle += dt * turn_speed;
	}

	xform.setRotation(btQuaternion(btVector3(0.0, 1.0, 0.0), turn_angle));

	btVector3 linearVelocity = rigid_body->getLinearVelocity();
	btScalar speed = rigid_body->getLinearVelocity().length();

	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();
	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btScalar walkSpeed = walk_speed * dt;

	if (forward)
	{
		walkDirection += forwardDir;
	}
	if (backward)
	{
		walkDirection -= forwardDir;
	}


	if (!forward && !backward && on_ground())
	{
		// Dampen when on the ground and not being moved by the player
		linearVelocity *= btScalar(0.2);
		rigid_body->setLinearVelocity(linearVelocity);
	}
	else
	{
		if (speed < max_linear_velocity)
		{
			btVector3 velocity = linearVelocity + walkDirection * walkSpeed;
			rigid_body->setLinearVelocity(velocity);
		}
	}

	rigid_body->getMotionState()->setWorldTransform(xform);
	rigid_body->setCenterOfMassTransform(xform);
}

bool noob::character_controller::can_jump() const
{
	return on_ground();
}

void noob::character_controller::jump()
{
	if (!can_jump())
	{
		return;
	}
	btTransform xform;
	rigid_body->getMotionState()->getWorldTransform(xform);
	btVector3 up = xform.getBasis()[1];
	up.normalize ();
	btScalar magnitude = (btScalar(1.0)/rigid_body->getInvMass()) * btScalar(8.0);
	rigid_body->applyCentralImpulse(up * magnitude);
}

bool noob::character_controller::on_ground() const
{
	return ray_lambda[0] < btScalar(1.0);
}

*/
