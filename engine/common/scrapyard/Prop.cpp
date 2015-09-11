#include "Prop.hpp"
#include "TransformHelper.hpp"

void noob::prop::init(btRigidBody* _body, noob::drawable* _drawable)
{
	body = _body;
	drawable = _drawable;
}

void noob::prop::set_position(const noob::vec3& _pos)
{
	btTransform t = body->getWorldTransform();
	t.setOrigin(btVector3(_pos.v[0], _pos.v[1], _pos.v[2]));
	body->setWorldTransform(t);
}


void noob::prop::set_orientation(const noob::versor& _orient)
{
	btTransform t = body->getWorldTransform();
	t.setRotation(btQuaternion(_orient.q[0], _orient.q[1], _orient.q[2], _orient.q[3]));
	body->setWorldTransform(t);
}


noob::mat4 noob::prop::get_transform() const
{
	noob::transform_helper t;
	t.translate(get_position());
	t.rotate(get_orientation());
	t.scale(drawing_scale);
	return t.get_matrix();
}


noob::vec3 noob::prop::get_position() const
{
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	return trans.getOrigin();
}


noob::versor noob::prop::get_orientation() const
{
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	return trans.getRotation();
}


std::string noob::prop::get_debug_info() const
{
	fmt::MemoryWriter w;
	noob::vec3 pos = get_position();
	noob::vec3 linear_vel = get_linear_velocity();
	noob::vec3 angular_vel = get_angular_velocity();
	w << "[Prop] Position: (" << pos.v[0] << ", " << pos.v[1] << ", " << pos.v[2] << "). Linear velocity: (" << linear_vel.v[0] << ", " << linear_vel.v[1] << ", " << linear_vel.v[2] << "). Angular Velocity: (" << angular_vel.v[0] << ", " << angular_vel.v[1] << ", " << angular_vel.v[2] << ")";
	return w.str();
}
