#include "PhysicsBody.hpp"

void noob::physics_body::init(const noob::mat4& transform, const noob::physics_shape& shape, float mass, float friction, float rolling_friction, float restitution)
{
	btTransform bt_trans;
	bt_trans.setFromOpenGLMatrix(&transform.m[0]);
	btDefaultMotionState* motion_state = new btDefaultMotionState(bt_trans);
	btCollisionShape* raw_shape = shape.get_shape();
	btVector3 local_inertia;
	raw_shape->calculateLocalInertia(mass, local_inertia);
	btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state, raw_shape, local_inertia);
	body = new btRigidBody(construction_info);
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
