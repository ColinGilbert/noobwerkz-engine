// This represents a conceptual geometric shape, using the physics engine under-the-hood.
// Limitation: btCollisionShape* inner_shape pointer must be delete'd by the application.
// Reason: The destructor used to do the above, but that unfortunately led to deletion of the btCollisionShape*  every time it went out of scope in a local block. This led to all sorts of fun.
// Comment: This breaks pure OOP principles but pure OOP isn't necessarily the best way forward. That goes doubly do when working with third party libraries.

#pragma once


#include <vector>
#include <memory>

#include <btBulletDynamicsCommon.h>
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"
#include "MathFuncs.hpp"


namespace noob
{
	class shape 
	{
		friend class application;
		friend class stage;
		friend class body;
		friend class ghost;
		friend class globals;
		
		public:
		enum class type { SPHERE, BOX, CYLINDER, CONE, HULL, TRIMESH };

		shape() noexcept(true) : physics_valid(false) {}

		void set_margin(float) noexcept(true);
		float get_margin() const noexcept(true);

		noob::shape::type get_type() const noexcept(true);

		protected:
		// Initializers. Used by noob::stage
		void sphere(float radius) noexcept(true);
		void box(float width, float height, float depth) noexcept(true);
		void cylinder(float radius, float height) noexcept(true);
		void capsule(float radius, float height) noexcept(true);
		void cone(float radius, float height) noexcept(true);
		void hull(const std::vector<noob::vec3>&) noexcept(true);
		void trimesh(const noob::basic_mesh&) noexcept(true);
		
		// void plane(const noob::vec3& normal, float offset) noexcept(true);
		
		// Used to calculate scale of world transform for drawing purposes (so that only one copy of the model gets kept in-buffer, which is a supremely useful optimization.)
		noob::vec3 get_scales() const noexcept(true);
		uint32_t index; // Hack to allow bodies to get shape handles from bodies and ghosts,
		noob::shape::type shape_type;
		bool physics_valid;
		noob::vec3 scales;
		std::array<float, 4> dims; // Hack to allow finding out model dimensions
		btCollisionShape* inner_shape;
	};
}
