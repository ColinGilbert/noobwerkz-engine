#include "PhysicsBody.hpp"
#include "Logger.hpp"
//#include "format.h"

void noob::physics_body::init(const noob::mat4& transform, const std::shared_ptr<noob::physics_shape>& _shape, float mass, float friction, float rolling_friction, float restitution)
{
	dirty = false;
	btTransform bt_trans;
	bt_trans.setFromOpenGLMatrix(&transform.m[0]);
	btDefaultMotionState* motion_state = new btDefaultMotionState(bt_trans);
	shape = _shape;
	btCollisionShape* raw_shape = shape->get_raw_ptr();
	btVector3 local_inertia(0.0, 0.0, 0.0);
	raw_shape->calculateLocalInertia(mass, local_inertia);
	btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state, raw_shape, local_inertia);
	body = new btRigidBody(construction_info);
/*
	btCollisionShape* fallShape = new btSphereShape(1);
        btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
        btScalar __mass = 1;
        btVector3 fallInertia(0, 0, 0);
        fallShape->calculateLocalInertia(__mass, fallInertia);
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(__mass, fallMotionState, fallShape, fallInertia);
        body = new btRigidBody(fallRigidBodyCI);
*/
}


void noob::physics_body::set_damping(float linear_damping, float angular_damping)
{
	body->setDamping(linear_damping, angular_damping);
}


void noob::physics_body::set_sleeping_thresholds(float linear, float angular)
{
	body->setSleepingThresholds(linear, angular);
}


void noob::physics_body::set_linear_factor(const noob::vec3& factor)
{
	body->setLinearFactor(btVector3(factor.v[0], factor.v[1], factor.v[2]));
}


void noob::physics_body::set_angular_factor(const noob::vec3& factor)
{
	body->setAngularFactor(btVector3(factor.v[0], factor.v[1], factor.v[2]));
}


void noob::physics_body::apply_force(const noob::vec3& force, const noob::vec3& relative_pos)
{
	body->applyForce(btVector3(force.v[0], force.v[1], force.v[2]), btVector3(relative_pos.v[0], relative_pos.v[1], relative_pos.v[2]));
}


void noob::physics_body::apply_torque(const noob::vec3& torque)
{
	body->applyTorque(btVector3(torque.v[0], torque.v[1], torque.v[2]));
}


void noob::physics_body::apply_impulse(const noob::vec3& impulse, const noob::vec3& relative_pos)
{
	body->applyForce(btVector3(impulse.v[0], impulse.v[1], impulse.v[2]), btVector3(relative_pos.v[0], relative_pos.v[1], relative_pos.v[2]));

}


void noob::physics_body::apply_torque_impulse(const noob::vec3& torque)
{
	body->applyTorqueImpulse(btVector3(torque.v[0], torque.v[1], torque.v[2]));

}


noob::mat4 noob::physics_body::get_transform() const
{
	const btMotionState* motion_state = body->getMotionState();
	btTransform bt_transform;
	motion_state->getWorldTransform(bt_transform);
	noob::mat4 mat;
	bt_transform.getOpenGLMatrix(&mat.m[0]);
	return mat;
}


btRigidBody* noob::physics_body::get_raw_ptr() const
{
	return body;
}


void noob::physics_body::print_debug_info() const
{
	//const btMotionState* motion_state = body->getMotionState();
	fmt::MemoryWriter w;
	btVector3 linear_vel = body->getLinearVelocity();
	btVector3 angular_vel = body->getAngularVelocity();
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	w << "[Body] Debug info: Position = (" << trans.getOrigin().getX() << ", " << trans.getOrigin().getY() << ", " << trans.getOrigin().getZ() << ") , linear velocity = "  << linear_vel.x() << ", " << linear_vel.y() << ", " << linear_vel.z() << "), angular velocity = (" << angular_vel.x() << ", " << angular_vel.y() << ", " << angular_vel.z() << ")";
	logger::log(w.str());
}
