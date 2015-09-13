#include "Body.hpp"


void noob::body::init(btDynamicsWorld* _dynamics_world, const noob::shape* _shape, float mass, const noob::vec3& position, const noob::versor& orientation)
{
	btTransform start_transform;
 	start_transform.setIdentity();
 	start_transform.setOrigin(btVector3(position.v[0], position.v[1], position.v[2]));
	start_transform.setRotation(btQuaternion(orientation.q[0], orientation.q[1], orientation.q[2], orientation.q[3]));
 	btVector3 inertia(0, 0, 0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	_shape->get_raw_ptr()->calculateLocalInertia(mass, inertia);
 	btRigidBody::btRigidBodyConstructionInfo ci(mass, motion_state, _shape->get_raw_ptr(), inertia);
	inner_body = new btRigidBody(ci);
	_dynamics_world->addRigidBody(inner_body);
}


void noob::body::init(btDynamicsWorld* _dynamics_world, const noob::shape* _shape, const noob::body::info& _info)
{

	btTransform start_transform;
 	start_transform.setIdentity();
 	start_transform.setOrigin(btVector3(_info.position.v[0], _info.position.v[1], _info.position.v[2]));
	start_transform.setRotation(btQuaternion(_info.orientation.q[0], _info.orientation.q[1], _info.orientation.q[2], _info.orientation.q[3]));
 	btVector3 inertia(0, 0, 0);
	btDefaultMotionState* motion_state = new btDefaultMotionState(start_transform);
	_shape->get_raw_ptr()->calculateLocalInertia(_info.mass, inertia);
 	btRigidBody::btRigidBodyConstructionInfo ci(_info.mass, motion_state, _shape->get_raw_ptr(), inertia);
	inner_body = new btRigidBody(ci);
	_dynamics_world->addRigidBody(inner_body);

	inner_body->setFriction(_info.friction);
	inner_body->setRestitution(_info.restitution);
	inner_body->setAngularFactor(btVector3(_info.angular_factor.v[0], _info.angular_factor.v[1], _info.angular_factor.v[2]));
	inner_body->setLinearFactor(btVector3(_info.linear_factor.v[0], _info.linear_factor.v[1], _info.linear_factor.v[2]));
	inner_body->setLinearVelocity(btVector3(_info.linear_velocity.v[0], _info.linear_velocity.v[1], _info.linear_velocity.v[2]));
	inner_body->setAngularVelocity(btVector3(_info.angular_velocity.v[0], _info.angular_velocity.v[1], _info.angular_velocity.v[2]));
}


void noob::body::set_position(const noob::vec3& _pos)
{
	btTransform t = inner_body->getWorldTransform();
	t.setOrigin(btVector3(_pos.v[0], _pos.v[1], _pos.v[2]));
	inner_body->setWorldTransform(t);
}


void noob::body::set_orientation(const noob::versor& _orient)
{
	btTransform t = inner_body->getWorldTransform();
	t.setRotation(btQuaternion(_orient.q[0], _orient.q[1], _orient.q[2], _orient.q[3]));
	inner_body->setWorldTransform(t);
}


noob::mat4 noob::body::get_transform() const
{
	noob::transform_helper t;
	t.translate(get_position());
	t.rotate(get_orientation());
	return t.get_matrix();
}


noob::vec3 noob::body::get_position() const
{
	btTransform trans;
	inner_body->getMotionState()->getWorldTransform(trans);
	return trans.getOrigin();
}


noob::versor noob::body::get_orientation() const
{
	btTransform trans;
	inner_body->getMotionState()->getWorldTransform(trans);
	return trans.getRotation();
}


noob::vec3 noob::body::get_linear_velocity() const
{
	return inner_body->getLinearVelocity();
}


noob::vec3 noob::body::get_angular_velocity() const
{
	return inner_body->getAngularVelocity();
}


std::string noob::body::get_debug_string() const
{
	fmt::MemoryWriter w;
	noob::vec3 pos = get_position();
	noob::vec3 linear_vel = get_linear_velocity();
	noob::vec3 angular_vel = get_angular_velocity();
	w << "[Prop] Position: (" << pos.v[0] << ", " << pos.v[1] << ", " << pos.v[2] << "). Linear velocity: (" << linear_vel.v[0] << ", " << linear_vel.v[1] << ", " << linear_vel.v[2] << "). Angular Velocity: (" << angular_vel.v[0] << ", " << angular_vel.v[1] << ", " << angular_vel.v[2] << ")";
	return w.str();
}


btRigidBody* noob::body::get_raw_ptr() const
{
	return inner_body;
}


noob::body::info noob::body::get_info() const
{
	noob::body::info results;
	results.init(inner_body);
	return results;
}
