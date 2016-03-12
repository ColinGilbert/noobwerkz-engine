#pragma once

#include "Shape.hpp"
#include <btBulletDynamicsCommon.h>
// #include <btBulletCollisionCommon.h>//""
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


namespace noob
{
	class ghost
	{
		public:
			ghost() : is_physical(false) {}
			
			void init(btCollisionWorld*, const noob::shape*, const noob::vec3&, const noob::versor&);

			void set_position(const std::vector<noob::vec3>&);
			void set_orientation(const std::vector<noob::versor>&);
		
			struct intersection_results
			{
				std::vector<size_t> bodies;
				std::vector<size_t> ghosts;
			};

			noob::ghost::intersection_results intersecting() const;

			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::mat4 get_transform() const;

			const bool is_physical;

		protected:
			btPairCachingGhostObject* inner;
			btCollisionWorld* world;
			// This is a hack to allow Bullet to introspect whether this is a physical or ghost object. Useful when filtering collisions between bodies and ghosts.
	};
}
