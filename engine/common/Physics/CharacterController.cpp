#include "CharacterController.hpp"



void noob::character_controller::init(const noob::physics_world& world, const std::shared_ptr<noob::physics_shape>& shape, const noob::mat4& position, float mass, float max_speed, float step_height)
{

}



void noob::character_controller::pre_step()
{

}


void noob::character_controller::player_step(float dt, bool forwards, bool backwards, bool left, bool right, bool jump)
{


}


void noob::character_controller::jump()
{

}


bool noob::character_controller::on_ground() const
{
	return false;
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
walk_velocity = 8.0; // meters/sec
turn_velocity = 1.0; // radians/sec
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
		turn_angle -= dt * turn_velocity;
	}
	if (right)
	{
		turn_angle += dt * turn_velocity;
	}

	xform.setRotation(btQuaternion(btVector3(0.0, 1.0, 0.0), turn_angle));

	btVector3 linearVelocity = rigid_body->getLinearVelocity();
	btScalar speed = rigid_body->getLinearVelocity().length();

	btVector3 forwardDir = xform.getBasis()[2];
	forwardDir.normalize();
	btVector3 walkDirection = btVector3(0.0, 0.0, 0.0);
	btScalar walkSpeed = walk_velocity * dt;

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
