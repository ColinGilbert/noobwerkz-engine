#include "Prop.hpp"

void noob::prop::init(rp3d::DynamicsWorld* w, const std::shared_ptr<noob::model>& drawable, const std::shared_ptr<noob::prepared_shaders::info>& uniforms, const noob::mat4& transform, noob::prop::type type)
{
	this->world = w;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&transform.m[0]));
	body = world->createRigidBody(t);
	shader_info = uniforms;
	//model = drawable;
	//body = world->createRigidBody(t)
	//this->world = w;
	set_transform(transform);

}

void noob::prop::set_transform(const noob::mat4& transform)
{
	rp3d::Transform _transform;
	_transform.setFromOpenGL(const_cast<rp3d::decimal*>(&transform.m[0]));
	body->setTransform(_transform);	
}


noob::mat4 noob::prop::get_transform() const
{
	noob::mat4 transform;
	body->getInterpolatedTransform().getOpenGLMatrix(&transform.m[0]);//transform();
	return transform;
}


void noob::prop::print_debug_info() const
{
	rp3d::Transform t = body->getInterpolatedTransform();
	fmt::MemoryWriter w;
	rp3d::Vector3 pos = t.getPosition();
	rp3d::Vector3 linear_vel = body->getLinearVelocity();
	rp3d::Vector3 angular_vel = body->getAngularVelocity();
	w << "[Character Controller] Position = (" <<  pos.x << ", " << pos.y << ", " << pos.z << "). Linear velocity = (" << linear_vel.x << ", " << linear_vel.y << ", " << linear_vel.z << "). Angular Velocity = (" << angular_vel.x << ", " << angular_vel.y << ", " << angular_vel.z << ")";
	logger::log(w.str());
}
