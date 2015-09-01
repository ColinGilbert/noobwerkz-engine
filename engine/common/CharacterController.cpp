#include "CharacterController.hpp"
#include "Logger.hpp"
#include <cmath>

void noob::character_controller::init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::prop>& _prop, float _width, float _height)
{
	width = _width;
	height = _height;
	world = _world;
	prop = _prop;

	rp3d::Material& material = prop->body->getMaterial();
	logger::log(fmt::format("[Character] bounce = {0}, friction = {1} ", material.getBounciness(), material.getFrictionCoefficient()));
	material.setBounciness(rp3d::decimal(0.0));
	prop->body->setType(rp3d::DYNAMIC);
}


void noob::character_controller::update()
{
	noob::vec3 from(prop->get_position().v[0], prop->get_position().v[1], prop->get_position().v[2]);
	noob::vec3 to(from.v[0], from.v[1] - ((height+1)/2.0), from.v[2]);

	rp3d::Ray ray(rp3d::Vector3(from.v[0], from.v[1], from.v[2]), rp3d::Vector3(to.v[0], to.v[1], to.v[2]));
	noob::character_controller::groundcast_callback ground_cb(prop.get(), from, to);
	world->raycast(ray, &ground_cb);
	logger::log("Raycasting!");
	if (ground_cb.is_grounded()) 
	{
		//logger::log("Grounded");
		stop();
		slope = ground_cb.get_slope();
		prop->body->setType(rp3d::KINEMATIC);
		self_control = true;
	}
	else 
	{
		//logger::log("Not grounded");
		prop->body->setType(rp3d::DYNAMIC);
		slope = noob::vec3(0.0, 0.0, 0.0);
		self_control = false;
	}
}


void noob::character_controller::step(bool forward, bool back, bool left, bool right, bool jump)
{
	float move_factor = 10.0;
	float jump_force = 20.0;

	if (self_control)
	{
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
		
		prop->body->setLinearVelocity(rp3d::Vector3(current_vel.v[0], current_vel.v[1], current_vel.v[2]));
	}
}


void noob::character_controller::stop()
{
	prop->body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));		
	prop->body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
}


rp3d::decimal noob::character_controller::groundcast_callback::notifyRaycastHit(const rp3d::RaycastInfo& info)
{
	grounded = true;
	//noob::vec3 hit_point = noob::vec3(info.worldPoint.x, info.worldPoint.z, info.worldPoint.y);
	slope = info.worldNormal;
	//float d = std::sqrt(noob::get_squared_dist(from, hit_point));
	//if (std::fabs(d) > std::fabs(cast_distance))
	//{
	//	airborne = false;
		logger::log("[Character] touching ground");
	//}
	return 0.0;
}
