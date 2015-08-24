#include "CharacterController.hpp"
#include "Logger.hpp"

void noob::character_controller::init(rp3d::DynamicsWorld* world, const noob::mat4& transform, float mass, float width, float height, float max_speed)
{
	this->width = width;
	this->height = height;
	this->mass = mass;
	this->max_speed = max_speed;
	this->world = world;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&transform.m[0]));
	body = world->createRigidBody(t);
	rp3d::CapsuleShape capsule(width/2, height);
	proxy_shape = body->addCollisionShape(capsule, rp3d::Transform::identity(), this->mass);
}


noob::mat4 noob::character_controller::get_transform() const 
{
	noob::mat4 mm;
	rp3d::Transform t = body->getInterpolatedTransform();
	t.getOpenGLMatrix(&mm.m[0]);
	return mm;
}

void noob::character_controller::print_debug_info() const
{
	rp3d::Transform t = body->getInterpolatedTransform();
	fmt::MemoryWriter w;
	rp3d::Vector3 pos = t.getPosition();
	rp3d::Vector3 linear_vel = body->getLinearVelocity();
	rp3d::Vector3 angular_vel = body->getAngularVelocity();
	w << "[Character Controller] Position = (" <<  pos.x << ", " << pos.y << ", " << pos.z << "). Linear velocity = (" << linear_vel.x << ", " << linear_vel.y << ", " << linear_vel.z << "). Angular Velocity = (" << angular_vel.x << ", " << angular_vel.y << ", " << angular_vel.z << ")";
	logger::log(w.str());
}
