#pragma once

#include <memory>
#include <btBulletDynamicsCommon.h>
#include "MathFuncs.hpp"


namespace noob
{
	class physics_shape
	{
		public:
			physics_shape() : created(false), type(NOT_INITIALIZED) {}
			~physics_shape() { delete shape; }
			
			enum shape_type
			{
				SPHERE, BOX, CYLINDER, CAPSULE, CONE, CONVEX, COMPOUND, MULTISPHERE, TRIMESH, PLANE, NOT_INITIALIZED
			};

			void set_sphere(float radius);

			void set_box(float width, float height, float depth);

			void set_cylinder(float radius, float height);

			void set_capsule(float radius, float height);

			void set_cone(float radius, float height);

			void set_convex_hull(const std::vector<noob::vec3>& points);

			void set_compound(const std::vector<std::shared_ptr<noob::physics_shape>>& shapes);

			void set_multisphere(const std::vector<noob::vec4>& pos_radii);

			void set_static_mesh(const std::vector<noob::vec3>& vertices, const std::vector<uint16_t>& indices);
		
			void set_static_plane(const noob::vec3& normal, float constant);

			noob::physics_shape::shape_type get_type() const;

			btCollisionShape* get_raw_ptr() const;



		protected:
			bool created;
			btCollisionShape* shape;
			shape_type type;
	};
}
