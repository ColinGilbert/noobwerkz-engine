#pragma once

#include <noob/component/component.hpp>

#include "Shape.hpp"
#include "Body.hpp"
#include "Ghost.hpp"
#include "Timing.hpp"
#include "Joint.hpp"

namespace noob
{
	class physics
	{
		public:
		typedef noob::handle<noob::shape> shape_handle;
		typedef noob::handle<noob::body> body_handle;
		typedef noob::handle<noob::ghost> ghost_handle;
		
		~physics() noexcept(true);
		
		void init(const noob::vec3& gravity, noob::duration timestep) noexcept(true);
		void step(noob::duration_fp) noexcept(true);
		void clear() noexcept(true);

		void set_gravity(const noob::vec3&) noexcept(true);
		noob::vec3 get_gravity() const noexcept(true);

		noob::physics::body_handle add_body(noob::body_type, const noob::physics::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true);
		noob::physics::body_handle add_body(noob::body_type, const noob::physics::shape_handle, const noob::body::info&) noexcept(true);		
		noob::physics::ghost_handle add_ghost(const noob::physics::shape_handle, const noob::vec3&, const noob::versor&) noexcept(true);
		
		noob::body& get_body(noob::physics::body_handle) noexcept(true);
		noob::ghost& get_ghost(noob::physics::ghost_handle) noexcept(true);

		protected:
		std::vector<noob::body> bodies;
		std::vector<noob::ghost> ghosts;
		std::vector<noob::joint> joints;

		btBroadphaseInterface* broadphase;
		btDefaultCollisionConfiguration* collision_configuration;
		btCollisionDispatcher* collision_dispatcher;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamics_world;

		void remove_body(noob::physics::body_handle) noexcept(true);
		void remove_ghost(noob::physics::ghost_handle) noexcept(true);
	};
}
