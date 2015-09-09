#pragma once
#include "MathFuncs.hpp"
#include "TransformHelper.hpp"

#include <btBulletDynamicsCommon.h>

namespace noob
{
	class physics_node
	{
		public:
			void init(btRigidBody*);
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);
			
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;
			
			std::string get_debug_info() const;

		protected:
			btRigidBody* body;
	};
}
