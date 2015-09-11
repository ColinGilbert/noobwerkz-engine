// A piece of scenery is simply a static physics body, its mesh shape, and a model associated.
#pragma once

#include <memory>

#include "Model.hpp"
#include "MathFuncs.hpp"
#include <btBulletDynamicsCommon.h>
#include "Drawable.hpp"

namespace noob
{
	class scenery
	{
		friend class stage;
		public:
			void init(btDiscreteDynamicsWorld* _world, noob::drawable*, const noob::vec3& _position, const noob::versor& _orientation);

			noob::mat4 get_transform() const { return transform; }

		protected:
			btRigidBody* body;
			btBvhTriangleMeshShape* shape;
			btDiscreteDynamicsWorld* world;
			noob::drawable* drawable;
			noob::mat4 transform;

	};
}
