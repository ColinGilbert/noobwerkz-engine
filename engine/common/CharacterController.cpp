/*
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
	//body->setType(rp3d::KINEMATIC);
	prop.init(world, this->model, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& = noob::identity_mat4(), noob::prop::type = noob::prop::type::DYNAMIC);

	rp3d::CapsuleShape capsule(width/2, height);
	prop.init()
	prop.get_body()->addCollisionShape(capsule, rp3d::Transform::identity(), this->mass);
}
*/
