// Kinematic, until self_control == false. The it beomes dynamic body
#pragma once


#include "MathFuncs.hpp"
#include "reactphysics3d.h"
#include "Prop.hpp"

namespace noob
{
	class character_controller
	{
		public:
			character_controller() : self_control(true) {}
			void init(rp3d::DynamicsWorld* _world, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4&, float _mass, float _width, float _height, float _max_speed);
			noob::mat4 get_transform() const;
			void print_debug_info() const;
			noob::prop get_prop() const { return prop; }

		protected:
			bool self_control;
			float width, mass, height, max_speed;
			noob::prop prop;
			rp3d::ProxyShape* proxy_shape;
			rp3d::DynamicsWorld* world;
	};
}
