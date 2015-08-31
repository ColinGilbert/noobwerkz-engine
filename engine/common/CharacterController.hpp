// Kinematic, until self_control == false. The it beomes dynamic body
#pragma once


#include "MathFuncs.hpp"
#include "Prop.hpp"

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : resting(false), self_control(false), time_on_ground(0.0), slope(0.0, 0.0, 0.0) {}

			//void init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4&, float _mass, float _width, float _height, float _max_speed);
			void update();
			void step(bool forward, bool back, bool left, bool right, bool jump);
			void stop();
			noob::mat4 get_transform() const { return prop.get_transform(); }
			//void print_debug_info() const;
			noob::prop* get_prop() const { return const_cast<noob::prop*>(&prop); }

		protected:
			bool resting, self_control;
			float width, mass, height, current_speed, max_speed, time_on_ground;
			noob::vec3 slope;
			noob::prop prop;
			//rp3d::DynamicsWorld* world;
	};
}
