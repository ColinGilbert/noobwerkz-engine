// TODO: Implement all creation functions (*) and ensure that they take constructor args

#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>

#include <boost/variant.hpp>
//#include <boost/intrusive_ptr.hpp>

#include <btBulletDynamicsCommon.h>
#include "es/storage.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "ShaderVariant.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"
#include "Body.hpp"
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
			
			// basic_model basic_model(const noob::basic_mesh&);	
			// basic_model basic_model(const noob::shape);
			
			// Loads a serialized model (from cereal binary)
			// animated_model_component::handle animated_model(const std::string& filename);
			// skeleton_component::handle skeleton(const std::string& filename);

			// actor_component::handle actor(const body, const basic_model, const skeleton_component::handle, const noob::vec3& pos = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));
			// prop_component::handle prop(const body, const basic_model, const noob::vec3& pos = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));
			// scenery_component::handle scenery(const basic_model, const noob::vec3& pos = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));
			// body body(const shape, float mass, const noob::vec3& pos = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));

			// light_component::handle light(const noob::light&);
			// reflection_component::handle reflection(const noob::reflection&);
			// shader_component::handle shader(const noob::prepared_shaders::info&, const std::string& name);
			
			// These cache the shape for quick reuse.
			// noob::shape sphere(float r);
			// noob::shape box(float x, float y, float z);
			// noob::shape cylinder(float r, float h);
			// noob::shape cone(float r, float h);
			// noob::shape capsule(float r, float h);
			// noob::shape plane(const noob::vec3& normal, float offset);
			// noob::shape hull(const std::vector<noob::vec3>& points, const std::string& name);
			// noob::shape trimesh(const noob::basic_mesh& mesh, const std::string& name);

		protected:

			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// TODO: Test other data structures.
			std::unordered_map<float, noob::shape> spheres;
			std::map<std::tuple<float, float, float>, noob::shape> boxes;
			std::map<std::tuple<float, float>, noob::shape> cylinders;
			std::map<std::tuple<float, float>, noob::shape> cones;
			std::map<std::tuple<float, float>, noob::shape> capsules;
			std::map<std::tuple<float,float,float,float>, noob::shape> planes;

			es::storage pool;

	};
}
