#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>

#include "NoobUtils.hpp"
#include "Mesh.hpp"

#define BIT(x) (1<<(x))

namespace noob
{
	class physics_world
	{
		public:

			enum collision_type
			{
				NOTHING = 0,
				CHARACTER = BIT(0),
				TERRAIN = BIT(1)
				// BUILDING = BIT(2)
			};

			struct body_properties
			{
				body_properties() : collision_group(CHARACTER), collides_with(CHARACTER | TERRAIN), mass(1.0), linear_damping(0.0), angular_damping(0.0), linear_factor(1.0, 1.0, 1.0), angular_factor(1.0, 1.0, 1.0) {}
				short collision_group, collides_with;
				float mass, linear_damping, angular_damping;
				noob::vec3 linear_factor, angular_factor;
			};

			class character_controller
			{
				public:

				protected:
			};


			~physics_world();

			void init();
			void cleanup();
			void step(double delta);

			void apply_global_force(const noob::vec3& force, const btCollisionShape* shape, const noob::vec3& origin);

			btRigidBody* dynamic_body(noob::physics_world::body_properties props, const noob::vec4& rotation, const noob::vec3& translation, btCollisionShape* shape);

			btSphereShape* sphere(float radius);
			btBoxShape* box(float width, float height, float depth);
			btCylinderShape* cylinder(float radius, float height);
			btCapsuleShape* capsule(float radius, float height);
			btConeShape* cone(float radius, float height);

			btConvexHullShape* convex_hull(const std::vector<noob::vec3>& points);
			btConvexHullShape* convex_hull(const noob::mesh& mesh);

			btCompoundShape* compound_shape(const noob::mesh& mesh);
			btCompoundShape* compound_shape(const std::vector<noob::mesh>& convex_meshes);
			btCompoundShape* compound_shape(const std::vector<btCollisionShape*>& shapes);

			// Static shapes
			btStaticPlaneShape* plane(const noob::vec3& normal, float distance);
			btBvhTriangleMeshShape* static_mesh(const noob::mesh& mesh);

			// Conveniences
			//bool set_shape(const std::string&, const btCollisionShape*);
			//bool set_body(const std::string&, const btRigidBody*);
			//std::tuple<bool, const btCollisionShape*> get_shape(const std::string&);
			//std::tuple<bool, const btRigidBody*> get_body(const std::string&);
			
			btDiscreteDynamicsWorld* dynamics_world;
			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* dispatcher;
			btSequentialImpulseConstraintSolver* solver;	

		protected:
			//std::map<const std::string, const btCollisionShape*> named_shapes;
			//std::map<const std::string, const btRigidBody*> named_bodies;


	};
}
