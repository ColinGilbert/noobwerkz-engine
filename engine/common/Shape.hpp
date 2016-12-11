// This represents a conceptual geometric shape, using the physics engine under-the-hood.
// Limitation: btCollisionShape* inner_shape pointer must be delete'd by the application.
// Reason: The destructor used to do the above, but that unfortunately led to deletion of the btCollisionShape*  every time it went out of scope in a local block. This led to all sorts of fun.
#pragma once

#include <vector>
#include <memory>

#include <noob/math/math_funcs.hpp>

#include <rdestl/fixed_array.h>

#include <btBulletDynamicsCommon.h>

#include "NoobUtils.hpp"
#include "Mesh3D.hpp"
#include "MeshUtils.hpp"


namespace noob
{
	class shape 
	{
		friend class body;
		friend class ghost;

		public:
		enum class type { SPHERE, BOX, /* CYLINDER, CONE, */ HULL, TRIMESH };

		shape() noexcept(true) : physics_valid(false), scales(noob::vec3(1.0, 1.0, 1.0)) {}

		// Initializers
		void sphere(float radius) noexcept(true);
		void box(float width, float height, float depth) noexcept(true);
		void cylinder(float radius, float height) noexcept(true);
		void capsule(float radius, float height) noexcept(true);
		void cone(float radius, float height) noexcept(true);
		void hull(const std::vector<noob::vec3>&) noexcept(true);
		void trimesh(const noob::mesh_3d&) noexcept(true);

		void set_margin(float) noexcept(true);
		float get_margin() const noexcept(true);

		noob::shape::type get_type() const noexcept(true);

		noob::vec3 get_scales() const noexcept(true);

		noob::mesh_3d get_mesh() const noexcept(true);

		void clear() noexcept(true);

		void set_self_index(uint32_t) noexcept(true);
		uint32_t get_self_index() const noexcept(true);
		// const btCollisionShape* get_inner() const noexcept(true);
		// btCollisionShape* get_inner_mutable() const noexcept(true);	

		protected:
		// void plane(const noob::vec3& normal, float offset) noexcept(true);
		
		noob::shape::type shape_type;
		bool physics_valid;
		noob::vec3 scales;
		btCollisionShape* inner;
	};
}
