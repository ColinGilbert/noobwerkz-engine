#include "Prop.hpp"
#include "TransformHelper.hpp"

void noob::prop::init(btRigidBody* _body, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::prepared_shaders::info>& _shading)
{
	body = _body;	
	shading = _shading;
	model = _model;
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


noob::vec3 noob::prop::get_linear_velocity() const
{
	return body->getLinearVelocity();
}



noob::vec3 noob::prop::get_angular_velocity() const
{
	return body->getAngularVelocity();
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
