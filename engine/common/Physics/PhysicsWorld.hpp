#pragma once

#include <array>

#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btTriangleIndexVertexArray.h>
#include <string>
#include <unordered_map>
#include <memory>
#include "NoobUtils.hpp"
#include "Mesh.hpp"
#include "PhysicsBody.hpp"
#include "PhysicsConstraintGeneric.hpp"
#include "PhysicsShape.hpp"

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

			~physics_world();

			void init();
			void cleanup();
			void step(double delta);

			// TODO: Implement
			void apply_global_force(const noob::vec3& force, const noob::physics_shape& shape, const noob::vec3& origin);

			void add(const noob::physics_body& body, short collision_group, short collides_with);
			void add(const noob::physics_constraint_generic& constraint);
			void add(const noob::physics_shape& shape, const std::string& name);

			void remove(const noob::physics_body& body);
			void remove(const noob::physics_constraint_generic& constraint);

			btDynamicsWorld* get_raw_ptr() const;

		protected:
			std::unordered_map<std::string, std::shared_ptr<noob::physics_shape>> shapes;
			btDiscreteDynamicsWorld* dynamics_world;
			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* dispatcher;
			btSequentialImpulseConstraintSolver* solver;	

	};
}
