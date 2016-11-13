#pragma once

#include <noob/component/component.hpp>
#include "Timing.hpp"

#include "Shape.hpp"
#include "Body.hpp"
#include "Ghost.hpp"

namespace noob
{
	class physics
	{
		public:
		typedef noob::handle<noob::shape> shape_handle;
		typedef noob::handle<noob::body> body_handle;
		typedef noob::handle<noob::ghost> ghost_handle;

		void init(const noob::vec3& gravity, noob::duration timestep) noexcept(true);
		void clear() noexcept(true);

		void set_gravity(const noob::vec3&) noexcept(true);
		void set_timestep(const noob::duration) noexcept(true);
		noob::vec3 get_gravity() const noexcept(true);
		noob::duration get_timestep() const noexcept(true);

		noob::physics::body_handle add_body(noob::body_type, const noob::physics::shape_handle, float mass, const noob::vec3& position, const noob::versor& orientation, bool ccd) noexcept(true);
		noob::physics::body_handle add_body(noob::body_type, const noob::physics::shape_handle, const noob::body::info&) noexcept(true);		
		noob::physics::ghost_handle add_ghost(const noob::physics::shape_handle, const noob::vec3&, const noob::versor&) noexcept(true);
		
		void set_position(noob::physics::body_handle, const noob::vec3&) noexcept(true);
		void set_orientation(noob::physics::body_handle, const noob::versor&) noexcept(true);
		noob::vec3 get_position(noob::physics::body_handle) const noexcept(true);
		noob::versor get_orientation(noob::physics::body_handle) const noexcept(true);
		noob::mat4 get_transform(noob::physics::body_handle) const noexcept(true);
		noob::physics::shape_handle get_shape(noob::physics::body_handle) const noexcept(true);
		noob::vec3 get_linear_velocity(noob::physics::body_handle) const noexcept(true);
		noob::vec3 get_angular_velocity(noob::physics::body_handle) const noexcept(true);

		void set_position(noob::physics::ghost_handle, const noob::vec3&) noexcept(true);
		void set_orientation(noob::physics::ghost_handle, const noob::versor&) noexcept(true);
		noob::vec3 get_position(noob::physics::ghost_handle) const noexcept(true);
		noob::versor get_orientation(noob::physics::ghost_handle) const noexcept(true);
		noob::mat4 get_transform(noob::physics::ghost_handle) const noexcept(true);
		noob::physics::shape_handle get_shape(noob::physics::ghost_handle) const noexcept(true);

		protected:

		std::vector<noob::body> bodies;
		std::vector<noob::ghost> ghosts;
	};
}
