#pragma once

#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>

#include <btBulletDynamicsCommon.h>
#include "BasicMesh.hpp"

namespace noob
{
	namespace physics
	{
		class shape
		{
			public:
				~shape() { delete inner_shape; }
				btCollisionShape* get_shape() const { return inner_shape; }
			protected:
				btCollisionShape* inner_shape;
		};
	}
}
