#pragma once

#include <vector>
#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Stage.hpp"

namespace noob
{
	class body
	{
		friend class stage;
		public:
			void init(float mass = 1.0, float friction = -1.0, float restitution = -1.0);
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);
			
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;
			
			std::string get_debug_info() const;

			btRigidBody* get_body() const;

		protected:
			btRigidBody* body;
	};
}
