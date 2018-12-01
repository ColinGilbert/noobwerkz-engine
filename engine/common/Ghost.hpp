#pragma once


// External libs
#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

#include <noob/component/component.hpp>

#include "Shape.hpp"

namespace noob
{
	class ghost
	{
		friend class physics;

		public:
		ghost() noexcept(true) : inner(nullptr) {}


		void set_position(const noob::vec3f&) noexcept(true);
		void set_orientation(const noob::versorf&) noexcept(true);

		noob::vec3f get_position() const noexcept(true);
		noob::versorf get_orientation() const noexcept(true);
		noob::mat4f get_transform() const noexcept(true);

		void set_user_index_1(uint32_t) noexcept(true);
		void set_user_index_2(uint32_t) noexcept(true);
		uint32_t get_user_index_1() const noexcept(true);
		uint32_t get_user_index_2() const noexcept(true);

		uint32_t get_shape_index() const noexcept(true);		

		protected:

		void init(btCollisionWorld* world, const noob::shape&, const noob::vec3f&, const noob::versorf&) noexcept(true);
		btPairCachingGhostObject* inner;
	};

	typedef noob::handle<noob::ghost> ghost_handle;
}
