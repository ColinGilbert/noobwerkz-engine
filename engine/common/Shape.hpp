#pragma once

#include <vector>

#include <cereal/types/array.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/portable_binary.hpp>

#include <btBulletDynamicsCommon.h>
#include "BasicMesh.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	class shape
	{
		enum class type { SPHERE, BOX, CAPSULE, CYLINDER, CONE, CONVEX, TRIMESH, PLANE };

		public:
		shape() : valid(false), margin(-1.0) {}
		~shape() { delete inner_shape; }

		void sphere(float radius);
		void box(float width, float height, float depth);
		void cylinder(float radius, float height);
		void capsule(float radius, float height);
		void cone(float radius, float height);
		void convex(const std::vector<noob::vec3>& points);
		void trimesh(const noob::basic_mesh&);
		void plane(const noob::vec3& normal, float offset);

		btCollisionShape* get_shape() const { return inner_shape; }
		
		void set_margin(float);
		float get_margin() const;

		protected:
		union info
		{
			noob::shape::type type;
			noob::vec2 radius_height;
			noob::vec3 width_height_depth;
			std::tuple<noob::vec3, float> normal_offset;
			std::unique_ptr<noob::basic_mesh> mesh;
		};
		
		noob::shape::type _type;
		bool valid;
		float margin;
		btCollisionShape* inner_shape;
	};
}
