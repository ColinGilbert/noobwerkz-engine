#include "Prop.hpp"
#include "TransformHelper.hpp"
/*
void noob::prop::init(rp3d::DynamicsWorld* w, const std::shared_ptr<noob::model>& drawable, const std::shared_ptr<noob::prepared_shaders::info>& uniforms, const noob::mat4& transform, rp3d::BodyType type)
{
	this->world = w;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&transform.m[0]));
	body = world->createRigidBody(t);
	body->setType(type);
	shading = uniforms;
	model = drawable;
}
*/
/*
void noob::prop::set_transform(const noob::mat4& transform)
{
	rp3d::Transform _transform;
	_transform.setFromOpenGL(const_cast<rp3d::decimal*>(&transform.m[0]));
	body->setTransform(_transform);	
}
*/

noob::mat4 noob::prop::get_transform() const
{
/*	//rp3d::Vector3 rp = body->getInterpolatedTransform().getPosition();
	rp3d::Vector3 rp = body->getTransform().getPosition();
	//rp3d::Quaternion rq = body->getInterpolatedTransform().getOrientation();
	rp3d::Quaternion rq = body->getTransform().getOrientation();
	noob::vec3 p(rp.x, rp.y, rp.z);
	noob::versor q(rq.x, rq.y, rq.z, rq.w);
	noob::transform_helper t;
	t.translate(p);
	t.rotate(q);
	//t.scale(scale);
	return t.get_matrix();
*/
}


void noob::prop::set_drawing_scale(const noob::vec3& s)
{
	scale = s;
}


void noob::prop::print_debug_info() const
{
	//rp3d::Transform t = body->getInterpolatedTransform();
/*	rp3d::Transform t = body->getTransform();
	fmt::MemoryWriter w;
	rp3d::Vector3 pos = t.getPosition();
	rp3d::Vector3 linear_vel = body->getLinearVelocity();
	rp3d::Vector3 angular_vel = body->getAngularVelocity();
	w << "[Prop] Position: (" << pos.x << ", " << pos.y << ", " << pos.z << "). Linear velocity: (" << linear_vel.x << ", " << linear_vel.y << ", " << linear_vel.z << "). Angular Velocity: (" << angular_vel.x << ", " << angular_vel.y << ", " << angular_vel.z << ")";
	logger::log(w.str());
*/
}


noob::prop::info noob::prop::get_info() const
{
/*
	rp3d::Transform t = body->getTransform();

	noob::prop::info inf;

	rp3d::Vector3 p = t.getPosition();
	inf.position.v[0] = p.x;
	inf.position.v[1] = p.y;
	inf.position.v[2] = p.z;
	
	rp3d::Quaternion q = t.getOrientation();
	inf.orientation.q[0] = q.x;
	inf.orientation.q[1] = q.y;
	inf.orientation.q[2] = q.z;
	inf.orientation.q[3] = q.w;
	
	return inf;
	*/
}
