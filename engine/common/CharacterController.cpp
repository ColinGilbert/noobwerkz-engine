#include "CharacterController.hpp"
#include "Logger.hpp"

void noob::character_controller::init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>& _model, const std::shared_ptr<noob::prepared_shaders::info>& _info, const noob::mat4& _transform, float _mass, float _width, float _height, float _max_speed)
{
	world = _world;
	mass = _mass;
	width = _width;
	height = _height;
	max_speed = _max_speed;
	rp3d::Transform t;
	t.setFromOpenGL(const_cast<reactphysics3d::decimal*>(&_transform.m[0]));
	prop.init(world, _model, _info, _transform);
	rp3d::CapsuleShape capsule(width/2, height);
	prop.body->addCollisionShape(capsule, rp3d::Transform::identity(), mass);
	prop.body->setType(rp3d::KINEMATIC);
}
