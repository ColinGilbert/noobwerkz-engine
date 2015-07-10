#pragma once

#include "Drawable.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "TransformHelper.hpp"
//#include <btBulletDynamicsCommon.h>
#include <memory>

namespace noob
{
	struct actor
	{
		public:
		actor() : hits(100.0), base_damage(1.0), drawable(nullptr) {} // , shape(nullptr), body(nullptr) {} 
		virtual ~actor()
		{
			if (drawable != nullptr) delete drawable;
		};
		float hits, base_damage;
		noob::transform_helper xform;

		//noob::vec3 translation;
		//noob::vec4 rotation;
		std::string name;

		noob::drawable* drawable;
		//btRigidBody* body;
	};
	
}
