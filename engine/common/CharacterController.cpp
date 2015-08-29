#include "CharacterController.hpp"
#include "Logger.hpp"
#include <cmath>

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
	// prop.body->setCenterOfMassLocal(rp3d::Vector3(0.0, 0.0, 0.0));//-height/2, 0.0));
	rp3d::Material& material = prop.body->getMaterial();

	logger::log(fmt::format("[Character] bounce = {0}, friction = {1} ", material.getBounciness(), material.getFrictionCoefficient()));

	material.setBounciness(rp3d::decimal(0.0));
	rp3d::CapsuleShape capsule(width, height);
	prop.body->addCollisionShape(capsule, rp3d::Transform::identity(), mass);
//	prop.body->setType(rp3d::KINEMATIC);
	//rp3d::SphereShape s(height);
	//prop.body->addCollisionShape(s, rp3d::Transform::identity(), mass);
}


void noob::character_controller::update(float dt)
{
	noob::prop::info inf = prop.get_info();
	noob::vec3 from(inf.position.v[0], inf.position.v[1], inf.position.v[2]);
	noob::vec3 to(from.v[0], from.v[1] - 5.0, from.v[2]);

	/*
	   fmt::MemoryWriter ww;
	   ww << "[CharacterController] Raycasting from (" << from.v[0] << ", " << from.v[1] << ", " << from.v[2] << ") to (" << to.v[0] << ", " << to.v[1] << ", " << to.v[2] << ")";
	   logger::log(ww.str());
	   */

	rp3d::Ray ray(rp3d::Vector3(from.v[0], from.v[1], from.v[2]), rp3d::Vector3(to.v[0], to.v[1], to.v[2]));
	noob::character_controller::groundcast_callback ground_cb(height, &prop, from, to);
	world->raycast(ray, &ground_cb);

	if (ground_cb.is_grounded()) 
	{
		stop();
		time_on_ground += dt;
		slope = ground_cb.get_slope();
		prop.body->setType(rp3d::KINEMATIC);
		self_control = true;
	}
	else 
	{
		prop.body->setType(rp3d::DYNAMIC);
		time_on_ground = 0.0;
		slope = noob::vec3(0.0, 0.0, 0.0);
		self_control = false;
	}
/*
	if (time_on_ground > 0.0) 
	{
		self_control = true;
	}
*/
}


void noob::character_controller::step(float dt, bool forward, bool back, bool left, bool right, bool jump)
{
	update(dt);

	// TODO: Replace nested conditionals with something more succinct

	prop.body->setTransform(rp3d::Transform(prop.body->getTransform().getPosition(), rp3d::Quaternion(0.0, 0.0, 0.0, 1.0)));	
	
	float move_factor = dt * 200.0;
	float jump_force = 20.0;
	if (self_control)
	{

		prop.body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
		prop.body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
		noob::vec3 current_vel(0.0, 0.0, 0.0);

		if (forward)
		{
			logger::log("[Character] going forward");
			noob::vec3 direction = noob::cross(noob::vec3(0.0, 0.0, -move_factor), slope);
			current_vel += direction;
		}

		if (back)
		{
			logger::log("[Character] going backward");
			noob::vec3 direction = noob::cross(noob::vec3(0.0, 0.0, move_factor), slope);
			current_vel += direction;
		}

		if (left)
		{
			logger::log("[Character] going left");
			noob::vec3 direction = noob::cross(noob::vec3(-move_factor, 0.0, 0.0), slope);
			current_vel += direction;
		}

		if (right)
		{
			logger::log("[Character] going right");
			noob::vec3 direction = noob::cross(noob::vec3(move_factor, 0.0, 0.0), slope);
			current_vel += direction;
		}

		if (jump)
		{
			logger::log("[Character] jumping");
			current_vel += noob::vec3(0.0, jump_force, 0.0);
		}
		
		prop.body->setLinearVelocity(rp3d::Vector3(current_vel.v[0], current_vel.v[1], current_vel.v[2]));
	}
}

void noob::character_controller::stop()
{
	prop.body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));		
	prop.body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
}


rp3d::decimal noob::character_controller::groundcast_callback::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
	noob::vec3 hit_point = noob::vec3(info.worldPoint.x, info.worldPoint.z, info.worldPoint.y);
	slope = info.worldNormal;
	float d = std::sqrt(noob::get_squared_dist(from, hit_point));
	if (d > ideal_distance)
	{
		airborne = false;
		logger::log("[Character] touching ground");
	}
	return 0.0;
}
