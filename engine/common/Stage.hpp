// TODO: Implement all creation functions (add_*) and ensure that they take constructor args


#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <boost/variant.hpp>

#include "Config.hpp"
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
#include <btBulletDynamicsCommon.h>
#include "Shape.hpp"
#include "Component.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_model_component;	
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_model_component;
	// typedef noob::component<std::unique_ptr<noob::basic_mesh>> mesh_component;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeleton_component;
	typedef noob::component<noob::light> light_component;
	typedef noob::component<noob::reflection> reflection_component;
	typedef noob::component<noob::prepared_shaders::info> shader_component;
	typedef noob::component<std::unique_ptr<noob::shape>> shape_component;
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
			
			basic_model_component::handle add_basic_model(const noob::basic_mesh&);	
			// Loads a serialized model (from cereal binary)
			animated_model_component::handle add_animated_model(const std::string& filename);
			skeleton_component::handle add_skeleton(const std::string& filename);
			actor_component::handle add_actor(const basic_model_component::handle, const skeleton_component::handle, const noob::vec3&, const noob::versor& v = noob::versor(0.0, 0.0, 0.0, 1.0));
			prop_component::handle add_prop(const basic_model_component::handle, const noob::vec3&, const noob::versor&);
			scenery_component::handle add_scenery(const basic_model_component::handle, const noob::vec3&, const noob::versor&);
			light_component::handle add_light(const noob::light&);
			reflection_component::handle add_reflection(const noob::reflection&);
			shader_component::handle add_shader(const noob::prepared_shaders::info&, const std::string& name);
			
			// These cache the shape for reuse, as they are simple parametrics.
			// TODO: Since these are globals, make them static, and/or separate from the stage class.
		
			noob::shape_component::handle sphere(float r);
			noob::shape_component::handle box(float x, float y, float z);
			noob::shape_component::handle cylinder(float r, float h);
			noob::shape_component::handle cone(float r, float h);
			noob::shape_component::handle capsule(float r, float h);
			noob::shape_component::handle plane(const noob::vec3& normal, float offset);

			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively. Will provide name soon.
			noob::shape_component::handle hull(const std::vector<noob::vec3>& points, const std::string& name);
			noob::shape_component::handle trimesh(const noob::basic_mesh& mesh, const std::string& name);

			// scenery_component sceneries;
			light_component lights;
			reflection_component reflections;
			shader_component shaders;
			shape_component shapes;
			body_component bodies;
			basic_model_component basic_models;
			animated_model_component animated_models;
			skeleton_component skeletons;

			bool paused;

		protected:
			// Protected function(s):
			// void draw(noob::drawable*, const noob::mat4&) const;
			std::unordered_map<std::string, noob::shape_component::handle> names;
			// Protected members:
			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			std::unordered_map<float, noob::shape_component::handle> spheres;
			std::map<std::tuple<float, float, float>, noob::shape_component::handle> boxes;
			std::map<std::tuple<float, float>, noob::shape_component::handle> cylinders;
			std::map<std::tuple<float, float>, noob::shape_component::handle> cones;
			std::map<std::tuple<float, float>, noob::shape_component::handle> capsules;
			std::map<std::tuple<float,float,float,float>, noob::shape_component::handle> planes;

	};
}

