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
		friend class globals;
		
		public:
		enum class type { SPHERE, BOX, CYLINDER, CONE, HULL, TRIMESH };

		shape() : physics_valid(false) {}

		void set_margin(float);
		float get_margin() const;

		noob::shape::type get_type() const;

		protected:
		// Initializers. Used by noob::stage
		void sphere(float radius);
		void box(float width, float height, float depth);
		void cylinder(float radius, float height);
		void capsule(float radius, float height);
		void cone(float radius, float height);
		void hull(const std::vector<noob::vec3>&);
		void trimesh(const noob::basic_mesh&);
		
		// void plane(const noob::vec3& normal, float offset);
		
		// Used to calculate scale of world transform for drawing purposes (so that only one copy of the model gets kept in-buffer, which is a supremely useful optimization.)
		noob::vec3 get_scales() const;

		noob::shape::type shape_type;
		bool physics_valid;
		noob::vec3 scales;
		std::array<float, 4> dims;
		btCollisionShape* inner_shape;
	};
}
