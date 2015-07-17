#pragma once

#include <array>

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
			};

			struct body_properties
			{
				body_properties() : mass(1.0), linear_damping(0.0), angular_damping(0.0), linear_factor(1.0, 1.0, 1.0), angular_factor(1.0, 1.0, 1.0) {}
				float mass, linear_damping, angular_damping;
				noob::vec3 linear_factor, angular_factor;
				btCollisionShape* shape;
			};
			
			struct generic_constraint_properties
			{
				std::array<btRigidBody*, 2> bodies;
				std::array<bool, 6> spring;
				std::array<bool, 6> motor;
				std::array<bool, 6> servo;
				std::array<float, 6> target_velocity;
				std::array<float, 6> servo_target;
				std::array<float, 6> max_motor_force;
				std::array<float, 6> stiffness;
				std::array<float, 6> damping;
				std::array<float, 6> bounce;
				std::array<noob::vec2, 6> limits;
				std::array<noob::mat4, 2> frames;
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

			// TODO: Implement
			void apply_global_force(const noob::vec3& force, btCollisionShape* shape, const noob::vec3& origin);
			
			btRigidBody* create_body(const noob::physics_world::body_properties& props, const noob::vec4& rotation, const noob::vec3& translation, btCollisionShape* shape);
			btGeneric6DofSpring2Constraint* create_constraint(const noob::physics_world::generic_constraint_properties& props);

			void add(btRigidBody* body, short collision_group = CHARACTER, short collides_with = CHARACTER | TERRAIN);
			void add(btGeneric6DofSpring2Constraint* constraint);

			void remove(btRigidBody* body);
			void remove(btGeneric6DofSpring2Constraint* constraint);

			btSphereShape* sphere(float radius);
			btBoxShape* box(float width, float height, float depth);
			btCylinderShape* cylinder(float radius, float height);
			btCapsuleShape* capsule(float radius, float height);
			btConeShape* cone(float radius, float height);

			btConvexHullShape* convex_hull(const std::vector<noob::vec3>& points);
			btConvexHullShape* convex_hull(const noob::mesh& mesh);

			// NOTE: This function is slow for large objects, as it calls V-HACD. If possible, use the latter two overloads during gameplay.
			btCompoundShape* compound_shape_decompose(const noob::mesh& mesh);
			btCompoundShape* compound_shape(const std::vector<noob::mesh>& convex_meshes);
			btCompoundShape* compound_shape(const std::vector<btCollisionShape*>& shapes);

			// Static shapes
			btStaticPlaneShape* plane(const noob::vec3& normal, float distance);
			btBvhTriangleMeshShape* static_mesh(const noob::mesh& mesh);


		protected:

			btDiscreteDynamicsWorld* dynamics_world;
			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* dispatcher;
			btSequentialImpulseConstraintSolver* solver;	

	};
}
