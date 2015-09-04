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
		public:
			scenery(btDiscreteDynamicsWorld* _world, const std::shared_ptr<noob::drawable>&, const noob::vec3& _position, const noob::versor& _orientation);

			noob::drawable* get_drawable() const { return drawable.get(); }
			noob::mat4 get_transform() const { return transform; }

		protected:
			btRigidBody* body;
			btBvhTriangleMeshShape* shape;
			btDiscreteDynamicsWorld* world;
			std::shared_ptr<noob::drawable> drawable;
			noob::mat4 transform;

	};
}
