#pragma once

#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Shape.hpp"
#include "Body.hpp"
#include "Ghost.hpp"
#include "Timing.hpp"
#include "Joint.hpp"
#include "ContactPoint.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"

namespace noob
{
	class physics
	{
		public:
		~physics() noexcept(true);
		
		void init() noexcept(true);
		void step(float) noexcept(true);
		void clear() noexcept(true);
		
		void set_gravity(const noob::vec3f&) noexcept(true);
		noob::vec3f get_gravity() const noexcept(true);
		
		noob::body_handle add_body(noob::body_type, const noob::shape_handle, float mass, const noob::vec3f& pos, const noob::versorf& orient, bool ccd) noexcept(true);
		noob::body_handle add_body(noob::body_type, const noob::shape_handle, const noob::body_info&) noexcept(true);		
		noob::ghost_handle add_ghost(const noob::shape_handle, const noob::vec3f&, const noob::versorf&) noexcept(true);
		
		noob::body& get_body(const noob::body_handle) noexcept(true);
		noob::ghost& get_ghost(const noob::ghost_handle) noexcept(true);
		
		// Oh crap... One of these kinds of functions...
		// You know the drill; the vector of contact points is passed to the function from calling scope and the number returned is how many contact points you've got.
		uint32_t get_intersecting(const noob::ghost_handle, std::vector<noob::contact_point>&) const noexcept(true);
		
		protected:
		// TODO: Move shapes here
		std::vector<noob::body> bodies;
		std::vector<noob::ghost> ghosts;
		std::vector<noob::joint> joints;
		
		btBroadphaseInterface* broadphase;
		btDefaultCollisionConfiguration* collision_configuration;
		btCollisionDispatcher* collision_dispatcher;
		btSequentialImpulseConstraintSolver* solver;
		btDiscreteDynamicsWorld* dynamics_world;
		
		void remove_body(const noob::body_handle) noexcept(true);
		void remove_ghost(const noob::ghost_handle) noexcept(true);
	};
}
