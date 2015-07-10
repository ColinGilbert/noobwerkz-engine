#pragma once

#include "PhysicsWorld.hpp"

namespace noob
{
	class character_controller
	{
		public:
			void init();
			noob::vec3 get_transform() const;
			void walk(const noob::vec3& position);
			void run(const noob::vec3& position);
			void strafe(const noob::vec3& position);

		protected:
			btGeneric6DofSpringConstraint* left_leg;
			btGeneric6DofSpringConstraint* right_leg;
			float max_speed;
			btRigidBody* character;
			btRigidBody* left_foot;
			btRigidBody* right_foot;
	};
}
