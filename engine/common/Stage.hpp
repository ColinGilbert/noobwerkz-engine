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
#include "Model.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
//#include "Prop.hpp"
//#include "Scenery.hpp"
#include "Component.hpp"

#include <btBulletDynamicsCommon.h>

#include "Sphere.hpp"
#include "Box.hpp"
#include "Capsule.hpp"
#include "Cylinder.hpp"
#include "Cone.hpp"
#include "Plane.hpp"
#include "Convex.hpp"

namespace noob
{
	// typedef noob::component<noob::prop>::handle prop_handle;
	// typedef noob::component<noob::actor>::handle actor_handle;
	// typedef noob::component<noob::scenery>::handle scenery_handle;
	typedef noob::component<std::unique_ptr<noob::model>>::handle model_handle;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>>::handle skeleton_handle;
	typedef noob::component<noob::light>::handle light_handle;
	typedef noob::component<noob::reflection>::handle reflection_handle;
	typedef noob::component<noob::prepared_shaders::info>::handle shaders_handle;
	// typedef noob::component<noob::physics_node>::handle body_handle;
	
	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);

			void draw() const;
			// Loads a serialized model (from cereal binary)
/*
			model_handle make_model(const std::string& filename);
			model_handle make_model(const noob::basic_mesh&);
			skeleton_handle make_skeleton(const std::string& filename);
			actor_handle make_actor(model_handle, skeleton_handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			prop_handle make_prop(btRigidBody*, size_t drawable, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			scenery_handle make_scenery(size_t drawable, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			light_handle make_light(const noob::light&);
			reflection_handle make_reflection(const noob::reflection&);
			shader_handle make_shader(const noob::prepared_shaders::info&);
*/



			// TODO: Replace with tags(?)
			btRigidBody* body(btCollisionShape*, float mass, const noob::vec3& pos, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));

			// These cache the shape for reuse, as they are simple parametrics.
			// TODO: Since these are globals, make them static, and/or separate from the stage class.
			btSphereShape* sphere(float r);
			btBoxShape* box(float x, float y, float z);
			btCylinderShape* cylinder(float r, float h);
			btConeShape* cone(float r, float h);
			btCapsuleShape* capsule(float r, float h);
			btStaticPlaneShape* plane(const noob::vec3& normal, float offset);

			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively. Might provide a way to cache frequently-used ones if needed.
			btConvexHullShape* hull(const std::vector<noob::vec3>& point);

			// noob::component<noob::prop> props;
			// noob::component<noob::actor> actors;
			// noob::component<noob::scenery> sceneries;
			noob::component<std::unique_ptr<noob::model>> models;
			noob::component<std::unique_ptr<noob::skeletal_anim>> skeletons;
			noob::component<noob::light> lights;
			noob::component<noob::reflection> reflections;
			noob::component<noob::prepared_shaders::info> shaders;
			// noob::component<btCollisionShape*> physics_shapes;
			// noob::component<btRigidBody*> physics_bodies;
			// Public member.
			bool paused;

		protected:
			// Protected function(s):
			// void draw(noob::drawable*, const noob::mat4&) const;

			// Protected members:
			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			std::unordered_map<float, btSphereShape*> spheres;
			std::map<std::tuple<float, float, float>, btBoxShape*> boxes;
			std::map<std::tuple<float, float>, btCylinderShape*> cylinders;
			std::map<std::tuple<float, float>, btConeShape*> cones;
			std::map<std::tuple<float, float>, btCapsuleShape*> capsules;
			std::map<std::tuple<float,float,float,float>, btStaticPlaneShape*> planes;


	};
}

