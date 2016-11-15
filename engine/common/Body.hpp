#pragma once

#include <btBulletDynamicsCommon.h>

#include "MathFuncs.hpp"
#include "Shape.hpp"


namespace noob
{
	class physics;
	class joint;

	enum class body_type {DYNAMIC = 0, KINEMATIC = 1, STATIC = 2};

	class body 
	{
		friend class physics;
		friend class joint;

		public:	
		body() noexcept(true) : physics_valid(false), ccd(false) {}

		struct info
		{
			noob::body_type type;
			float mass, friction, restitution;
			noob::vec3 position, linear_velocity, angular_velocity, linear_factor, angular_factor;
			noob::versor orientation;
			bool ccd;
		};

		void init(btDynamicsWorld* const, noob::body_type, const noob::shape&, float mass, const noob::vec3& position, const noob::versor& orientation, bool ccd) noexcept(true);
		void init(btDynamicsWorld* const, noob::body_type, const noob::shape&, const noob::body::info&) noexcept(true);

		// void set_type(noob::body_type) noexcept(true);

		void set_position(const noob::vec3&) noexcept(true);
		void set_orientation(const noob::versor&) noexcept(true);

		noob::vec3 get_position() const noexcept(true);
		noob::versor get_orientation() const noexcept(true);

		noob::mat4 get_transform() const noexcept(true);
		noob::vec3 get_linear_velocity() const noexcept(true);
		noob::vec3 get_angular_velocity() const noexcept(true);

		noob::body::info get_info() const noexcept(true);

		std::string get_debug_string() const noexcept(true);

		// At this point in time, we can only set ccd on.
		// TODO: Find out how to turn it off properly.
		void set_ccd(bool) noexcept(true); 
		bool get_ccd_enabled() const noexcept(true); 
		float get_ccd_threshold() const noexcept(true); 

		uint32_t get_shape_index() const noexcept(true);

		protected:
		noob::body_type type;
		bool physics_valid, ccd;
		btRigidBody* inner;
	};
}
