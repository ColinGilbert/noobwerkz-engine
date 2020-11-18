#pragma once

// External libs
#include <noob/math/math_funcs.hpp>
#include <btBulletDynamicsCommon.h>
#include <noob/component/component.hpp>
// Project-local
#include "Shape.hpp"
#include "BodyInfo.hpp"

namespace noob
{
	class physics;
	class joint;


	class body 
	{
		friend class physics;
		friend class constraint;
		public:	
		body() noexcept(true) : physics_valid(false), ccd(false) {}


		void init(btDynamicsWorld* const, noob::body_type, const noob::shape&, float mass, const noob::vec3f& position, const noob::versorf& orientation, bool ccd) noexcept(true);
		void init(btDynamicsWorld* const, const noob::body_info&) noexcept(true);

		// void set_type(noob::body_type) noexcept(true);

		void set_position(const noob::vec3f&) noexcept(true);
		void set_orientation(const noob::versorf&) noexcept(true);

		noob::vec3f get_position() const noexcept(true);
		noob::versorf get_orientation() const noexcept(true);

		void set_linear_factor(const noob::vec3f&) const noexcept(true);
		void set_angular_factor(const noob::vec3f&) const noexcept(true);

		noob::mat4f get_transform() const noexcept(true);
		noob::vec3f get_linear_velocity() const noexcept(true);
		noob::vec3f get_angular_velocity() const noexcept(true);

		noob::body_info get_info() const noexcept(true);

		std::string get_debug_string() const noexcept(true);

		// At this point in time, we can only set ccd on.
		// TODO: Find out how to turn it off properly.
		void set_ccd(bool) noexcept(true); 
		bool get_ccd_enabled() const noexcept(true); 
		float get_ccd_threshold() const noexcept(true); 

		void set_user_index_1(uint32_t) noexcept(true);
		void set_user_index_2(uint32_t) noexcept(true);
		uint32_t get_user_index_1() const noexcept(true);
		uint32_t get_user_index_2() const noexcept(true);

		// This will give you an offset into the array you store your shapes in, no matter where it may be.
		// Note: Only really useful if you store your shapes in one place.
		uint32_t get_shape_index() const noexcept(true);

		protected:
		noob::body_type type;
		bool physics_valid, ccd;
		btRigidBody* inner;
	};

	typedef noob::handle<noob::body> body_handle;
}
