// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Drawable.hpp"
#include "Stage.hpp"

namespace noob
{
	class prop
	{
		friend class stage;
		public:
			prop() : body(nullptr), drawing_scale(noob::vec3(1.0, 1.0, 1.0)) {}

			void init(btRigidBody*, noob::drawable*);
			
			void set_position(const noob::vec3&);
			void set_orientation(const noob::versor&);

			noob::mat4 get_transform() const;
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;

			noob::vec3 get_linear_velocity() const { return body->getLinearVelocity(); }
			noob::vec3 get_angular_velocity() const { return body->getAngularVelocity(); }
			std::string get_debug_info() const;
		protected:	
			btRigidBody* body;
			
			noob::drawable* drawable;
			noob::vec3 drawing_scale;
	};
}
