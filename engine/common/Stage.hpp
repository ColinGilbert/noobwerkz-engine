#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <boost/variant.hpp>

//#include "Drawable.hpp"
#include "Config.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "ShaderVariant.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
//#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
//#include "Prop.hpp"
//#include "Scenery.hpp"
#include "Body.hpp"
#include "ComponentDefines.hpp"
#include <btBulletDynamicsCommon.h>
#include "Shape.hpp"


namespace noob
{
	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);

			void draw() const;
/*
			// Loads a serialized model (from cereal binary)
			animated_model_handle model(const std::string& filename);
			mesh_handle mesh(const noob::basic_mesh&);
			skeleton_handle make_skeleton(const std::string& filename);
			actor_handle actor(model_handle, skeleton_handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			prop_handle prop(mesh_handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			scenery_handle scenery(mesh_handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			light_handle light(const noob::light&);
			reflection_handle reflection(const noob::reflection&);
			shader_handle shader(const noob::prepared_shaders::info&);
*/

			
			// These cache the shape for reuse, as they are simple parametrics.
			// TODO: Since these are globals, make them static, and/or separate from the stage class.
/*			
			btSphereShape* sphere(float r);
			btBoxShape* box(float x, float y, float z);
			btCylinderShape* cylinder(float r, float h);
			btConeShape* cone(float r, float h);
			btCapsuleShape* capsule(float r, float h);
			btStaticPlaneShape* plane(const noob::vec3& normal, float offset);
			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively. Might provide a way to cache frequently-used ones if needed.
			btConvexHullShape* hull(const std::vector<noob::vec3>& point);
*/
			
			// scenery_component sceneries;
			light_component lights;
			reflection_component reflections;
			shader_component shaders;
			shape_component shapes;
			body_component bodies;
			model_component models;
			mesh_component meshes;
			skeleton_component skeletons;

			bool paused;

		protected:
			// Protected function(s):
			// void draw(noob::drawable*, const noob::mat4&) const;

			// btRigidBody* body(btCollisionShape*, float mass, const noob::vec3& pos, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));

			// Protected members:
			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
/*
			std::unordered_map<float, btSphereShape*> spheres;
			std::map<std::tuple<float, float, float>, btBoxShape*> boxes;
			std::map<std::tuple<float, float>, btCylinderShape*> cylinders;
			std::map<std::tuple<float, float>, btConeShape*> cones;
			std::map<std::tuple<float, float>, btCapsuleShape*> capsules;
			std::map<std::tuple<float,float,float,float>, btStaticPlaneShape*> planes;
*/

	};
}

