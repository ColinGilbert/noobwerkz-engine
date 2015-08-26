/*
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
			void init(rp3d::DynamicsWorld* world, const noob::prop&, const noob::mat4&, float mass, float width, float height, float max_speed);
			noob::mat4 get_transform() const;
			void print_debug_info() const;
			noob::prop get_prop() const { return prop; }

		protected:
			bool self_control;
			float width, height, mass, max_speed;
			noob::prop prop;
			rp3d::ProxyShape* proxy_shape;
			rp3d::DynamicsWorld* world;
	};
}

*/
