#include "Body.hpp"


noob::body(const noob::shape* _shape, float mass, const noob::vec3& position, const noob::versor& orientation)
{
	
}


noob::body(const noob::shape* _shape, const noob::body::info&)
{

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
	t.scale(drawing_scale);
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


std::string noob::body::get_debug_info() const
{
	fmt::MemoryWriter w;
	noob::vec3 pos = get_position();
	noob::vec3 linear_vel = get_linear_velocity();
	noob::vec3 angular_vel = get_angular_velocity();
	w << "[Prop] Position: (" << pos.v[0] << ", " << pos.v[1] << ", " << pos.v[2] << "). Linear velocity: (" << linear_vel.v[0] << ", " << linear_vel.v[1] << ", " << linear_vel.v[2] << "). Angular Velocity: (" << angular_vel.v[0] << ", " << angular_vel.v[1] << ", " << angular_vel.v[2] << ")";
	return w.str();
}


btRigidBody* noob::body::get_body() const
{
	return inner_body;
}


noob::body::info noob::body::get_info() const
{
	noob::body::info results;
	results.init(inner_body);
	return results;
}
