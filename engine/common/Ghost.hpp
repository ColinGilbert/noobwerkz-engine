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
			void init(const noob::shape&, const noob::vec3&, const noob::versor&);

			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);

			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;

		protected:
			btPairCachingGhostObject* inner;
	};
}
