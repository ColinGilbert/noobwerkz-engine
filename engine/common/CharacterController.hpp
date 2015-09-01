// Kinematic, until self_control == false. The it beomes dynamic body
#pragma once


#include "MathFuncs.hpp"
#include "Prop.hpp"
#include <btBulletDynamicsCommon.h>
#include <memory>

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : resting(false), self_control(false), time_on_ground(0.0), slope(0.0, 0.0, 0.0), world(nullptr) {}

			void init(btDynamicsWorld*, const std::shared_ptr<noob::prop>&);
			void update();
			void step(bool forward, bool back, bool left, bool right, bool jump);
			void stop();
			noob::mat4 get_transform() const { return prop->get_transform(); }
			noob::vec3 get_position() const { return prop->get_position(); }
			noob::versor get_orientation() const { return prop->get_orientation(); }
			std::string get_debug_info() const { return prop->get_debug_info(); }
			noob::prop* get_prop() const { return prop.get(); }

		protected:
			bool resting, self_control;
			float width, mass, height, current_speed, max_speed, time_on_ground;
			noob::vec3 slope;
			std::shared_ptr<noob::prop> prop;
			btDynamicsWorld* world;
	};
}
