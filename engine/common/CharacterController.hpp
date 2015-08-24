#pragma once


#include "MathFuncs.hpp"
#include "reactphysics3d.h"

namespace noob
{
	class character_controller
	{
		public:
			void init(rp3d::DynamicsWorld* world, const noob::mat4&, float mass, float width, float height, float max_speed);
			noob::mat4 get_transform() const;
			void print_debug_info() const;
			

		protected:
			float width, height, mass, max_speed;
			rp3d::RigidBody* body;
			rp3d::ProxyShape* proxy_shape;
			rp3d::DynamicsWorld* world;
	};
}
