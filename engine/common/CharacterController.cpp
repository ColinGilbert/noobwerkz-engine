#include "CharacterController.hpp"
#include "Logger.hpp"
#include <cmath>

void noob::character_controller::init(btDynamicsWorld* _world, const std::shared_ptr<noob::prop>& _prop)
{
	world = _world;
	prop = _prop;
}


void noob::character_controller::update()
{
	noob::vec3 from(prop->get_position().v[0], prop->get_position().v[1], prop->get_position().v[2]);
	noob::vec3 to(from.v[0], from.v[1] - ((height+1)/2.0), from.v[2]);
}


void noob::character_controller::step(bool forward, bool back, bool left, bool right, bool jump)
{

	// TODO: Replace nested conditionals with something more succinct

/*	
	
	float move_factor = 10.0;
	float jump_force = 20.0;
	if (self_control)
	{
		// prop.body->setTransform(rp3d::Transform(prop.body->getTransform().getPosition(), rp3d::Quaternion(0.0, 0.0, 0.0, 1.0)));	
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
*/
}

void noob::character_controller::stop()
{
//	prop.body->setLinearVelocity(rp3d::Vector3(0.0, 0.0, 0.0));		
//	prop.body->setAngularVelocity(rp3d::Vector3(0.0, 0.0, 0.0));
}

