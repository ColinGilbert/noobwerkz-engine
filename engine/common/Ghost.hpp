#pragma once

#include "Shape.hpp"
// #include <btBulletDynamicsCommon.h>
// #include <btBulletCollisionCommon.h>//""
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


namespace noob
{
	class ghost
	{
		public:

			void init(const noob::shape*, const noob::vec3&, const noob::versor&);
		protected:
			btPairCachingGhostObject* inner;
	};
}
