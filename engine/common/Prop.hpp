// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once
#include <memory>
#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Drawable.hpp"


namespace noob
{
	class prop
	{
		public:
			prop() : body(nullptr), drawing_scale(noob::vec3(1.0, 1.0, 1.0)) {}

			void init(btRigidBody*, const std::shared_ptr<drawable>&);
		
			noob::drawable* get_drawable() const { return drawable.get(); }

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
			
			std::shared_ptr<noob::drawable> drawable;
			noob::vec3 drawing_scale;
	};
}
