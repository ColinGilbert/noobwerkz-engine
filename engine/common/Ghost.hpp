#pragma once


#include <btBulletDynamicsCommon.h>
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


#include "Shape.hpp"

namespace noob
{
	class ghost
	{
		friend class stage;

		public:
		void init(btCollisionWorld* const world, const noob::shape&, const noob::vec3&, const noob::versor&) noexcept(true);

		void set_position(const noob::vec3&) noexcept(true);
		void set_orientation(const noob::versor&) noexcept(true);

		noob::vec3 get_position() const noexcept(true);
		noob::versor get_orientation() const noexcept(true);
		noob::mat4 get_transform() const noexcept(true);

		protected:
		btPairCachingGhostObject* inner;
	};
}
