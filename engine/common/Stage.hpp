// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <atomic>

#include <boost/variant.hpp>

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
#include "Model.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"
#include "Body.hpp"
#include <btBulletDynamicsCommon.h>
#include "Shape.hpp"
#include "Component.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::model>> model_component;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeleton_component;
	typedef noob::component<std::unique_ptr<noob::shape>> shape_component;
	typedef noob::component<noob::light> light_component;
	typedef noob::component<noob::reflection> reflection_component;
	typedef noob::component<noob::prepared_shaders::info> shader_component;
	typedef noob::component<noob::body> body_component;
	typedef noob::component<noob::prop> prop_component;	
	typedef noob::component<noob::actor> actor_component;
	typedef noob::component<noob::scenery> scenery_component;

	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);
			
			void draw() const;
			
			noob::model_component::handle model(const noob::basic_mesh&);	
			noob::model_component::handle model(const noob::shape_component::handle);
			// Loads a serialized model (from cereal binary)
			noob::model_component::handle model(const std::string& filename);	
			
			noob::basic_mesh make_mesh(const noob::shape_component::handle h);
			
			noob::skeleton_component::handle skeleton(const std::string& filename);
			
			noob::actor_component::handle actor(const noob::prop_component::handle, const noob::skeleton_component::handle);
			noob::prop_component::handle prop(const noob::body_component::handle, const noob::model_component::handle, const noob::shader_component::handle);
			noob::prop_component::handle scenery(const noob::basic_mesh&, const noob::vec3& pos, const noob::versor& orient, const noob::shader_component::handle);
			noob::body_component::handle body(const noob::shape_component::handle, float mass, const noob::vec3& pos, const noob::versor& orient);
		
			void change_shading(noob::prop_component::handle, noob::shader_component::handle);

			noob::light_component::handle light(const noob::light&);
			noob::reflection_component::handle reflection(const noob::reflection&);
			noob::shader_component::handle shader(const noob::prepared_shaders::info&, const std::string& name);
			
			// These cache the shape for reuse, as they are simple parametrics.
			noob::shape_component::handle sphere(float r);
			noob::shape_component::handle box(float x, float y, float z);
			noob::shape_component::handle cylinder(float r, float h);
			noob::shape_component::handle cone(float r, float h);
			noob::shape_component::handle capsule(float r, float h);
			noob::shape_component::handle plane(const noob::vec3& normal, float offset);
			
			noob::shape_component::handle hull(const std::vector<noob::vec3>& points, const std::string& name);
			noob::shape_component::handle trimesh(const noob::basic_mesh& mesh, const std::string& name);
			
			noob::prop_component props;
			noob::actor_component actors;
			noob::light_component lights;
			noob::reflection_component reflections;
			noob::shader_component shaders;
			noob::shape_component shapes;
			noob::body_component bodies;
			noob::model_component models;
			noob::skeleton_component skeletons;
			
		protected:

			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// TODO: Test other data structures.
			std::unordered_map<float, noob::shape_component::handle> spheres;
			std::map<std::tuple<float, float, float>, noob::shape_component::handle> boxes;
			std::map<std::tuple<float, float>, noob::shape_component::handle> cylinders;
			std::map<std::tuple<float, float>, noob::shape_component::handle> cones;
			std::map<std::tuple<float, float>, noob::shape_component::handle> capsules;
			std::map<std::tuple<float,float,float,float>, noob::shape_component::handle> planes;
			std::map<shape_component::handle, body_component::handle> dependents_of_shape;
			//std::map<model_variant, std::tuple<light_component::handle, reflection_component::handle>> dependents_of_model;

			std::atomic<size_t> prop_counter, scenery_counter;
	};
}
