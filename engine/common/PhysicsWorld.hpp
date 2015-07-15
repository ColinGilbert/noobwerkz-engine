#pragma once

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <string>
#include <unordered_map>
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
			
			physics_world() : body_counter(0), shape_counter(0) {}
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
			size_t add_shape(const btCollisionShape*);
			size_t add_body(const btRigidBody*);

			bool add_body_name(size_t id, const std::string& name);
			bool add_shape_name(size_t id, const std::string& name);

			const btCollisionShape* get_shape(size_t);
			size_t get_shape_id(const std::string& name);

			const btRigidBody* get_body(size_t id);
			size_t get_body_id(const std::string& name);

			btDiscreteDynamicsWorld* dynamics_world;
			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* dispatcher;
			btSequentialImpulseConstraintSolver* solver;	

		protected:
			std::unordered_map<std::string, size_t> shape_names;
			std::unordered_map<size_t, const btCollisionShape*> shapes;
			std::unordered_map<std::string, size_t> body_names;
			std::unordered_map<size_t, const btRigidBody*> bodies;
			size_t body_counter, shape_counter;

	};
}
