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
	prop.body->setAngularDamping(4.0);
	rp3d::CapsuleShape capsule(width, height);
	prop.body->addCollisionShape(capsule, rp3d::Transform::identity(), mass);
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
	//on_ground = ;

	if (!ground_cb.is_grounded()) 
	{
		time_on_ground = 0.0;
		slope = noob::vec3(0.0, 0.0, 0.0);
		self_control = false;
	}

	else 
	{
		time_on_ground += dt;
		slope = ground_cb.get_slope();
	}

	if (time_on_ground > 0.25) 
	{
		self_control = true;
	}
}


void noob::character_controller::step(float dt, bool forward, bool back, bool left, bool right, bool jump)
{
	update(dt);

	// TODO: Replace nested conditionals with something more succinct
	
	float forward_factor = dt * 5.0;
	float backward_factor = dt * 2.5;
	float angle_factor = dt * 5.0;
	float jump_force = 10.0;
	if (self_control)
	{
				
		if (forward)
		{
		//	if (!back)
		//	{
				logger::log("[Character] going forward");
				noob::vec3 direction = noob::cross(slope, noob::vec3(forward_factor, 0.0, 0.0));
				prop.body->applyForceToCenterOfMass(rp3d::Vector3(direction.v[0], direction.v[1], direction.v[2]));
		//	}
		}
		
		if (back)
		{
		//	if (!forward)
		//	{
				logger::log("[Character] going backward");
				noob::vec3 direction = noob::cross(slope, noob::vec3(-backward_factor, 0.0, 0.0));
				prop.body->applyForceToCenterOfMass(rp3d::Vector3(direction.v[0], direction.v[1], direction.v[2]));

		//	}
		}
		
		if (left)
		{
		//	if (!right)
		//	{
				logger::log("[Character] going left");
				prop.body->applyTorque(rp3d::Vector3(0.0, 0.0, -angle_factor));
		//	}
		}
		
		if (right)
		{
		//	if (!left)
		//	{
				logger::log("[Character] going right");
				prop.body->applyTorque(rp3d::Vector3(0.0, 0.0, angle_factor));
		//	}
		}
		
		if (jump)
		{
			logger::log("[Character] jumping");
			prop.body->applyForceToCenterOfMass(rp3d::Vector3(0.0, jump_force, 0.0));
			self_control = false;
		}

	}
}


rp3d::decimal noob::character_controller::groundcast_callback::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
	noob::vec3 hit_point = noob::vec3(info.worldPoint.x, info.worldPoint.z, info.worldPoint.y);
	slope = info.worldNormal;
	float d = std::sqrt(noob::get_squared_dist(from, hit_point));
	if (d > ideal_distance)
	{
		airborne = false;
		// logger::log("[Character] touching ground");
	}
	return 1.0;
}
