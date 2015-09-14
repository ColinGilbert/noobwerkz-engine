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

#include <boost/variant/variant.hpp>
#include "inline_variant.hpp"

namespace noob
{
	class shape
	{
		public:
		enum class type { SPHERE, BOX, CAPSULE, CYLINDER, CONE, CONVEX, TRIMESH, PLANE };
		shape() : physics_valid(false), mesh_initialized(false), dims({ 0.0, 0.0, 0.0, 0.0 }), margin(-1.0) {}
		~shape() { delete inner_shape; }
		
		// For manual init
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

		btCollisionShape* get_raw_ptr() const;

		protected:
		noob::shape::type shape_type;
		bool physics_valid, mesh_initialized;
		std::array<float, 4> dims;
		float margin;
		btCollisionShape* inner_shape;
		noob::basic_mesh inner_mesh;
	};
}
