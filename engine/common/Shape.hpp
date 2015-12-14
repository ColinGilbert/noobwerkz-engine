// This represents a conceptual geometric shape.
// Tied into the physics engine.
#pragma once


#include <vector>
#include <memory>

#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <btBulletDynamicsCommon.h>
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"
#include "MathFuncs.hpp"



namespace noob
{
	class shape
	{
		friend class body;
		public:
		enum class type { SPHERE, BOX, CAPSULE, CYLINDER, CONE, CONVEX, TRIMESH, PLANE };
		shape() : physics_valid(false), mesh_initialized(false), dims({ 0.0, 0.0, 0.0, 0.0 }), margin(-1.0) {}
		~shape() { delete inner_shape; }

		// TODO: Make it able to swap shapes at runtime
		void sphere(float radius);
		void box(float width, float height, float depth);
		void cylinder(float radius, float height);
		void capsule(float radius, float height);
		void cone(float radius, float height);
		void convex(const std::vector<noob::vec3>& points);
		void trimesh(const noob::basic_mesh&);
		void plane(const noob::vec3& normal, float offset);

		void set_margin(float);
		float get_margin() const;

		protected:
		btCollisionShape* get_raw_ptr() const;
		noob::shape::type shape_type;
		bool physics_valid, mesh_initialized;
		std::array<float, 4> dims;
		float margin;
		btCollisionShape* inner_shape;
		noob::basic_mesh inner_mesh;
	};
}
