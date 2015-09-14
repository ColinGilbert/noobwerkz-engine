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
			
			
			basic_model_component::handle add_basic_model(const noob::basic_mesh&);	
			// Loads a serialized model (from cereal binary)
			animated_model_component::handle add_animated_model(const std::string& filename);
			skeleton_component::handle add_skeleton(const std::string& filename);
			//actor_component::handle actor(basic_model_component::handle, skeleton_component::handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			//prop_component::handle prop(mesh_component::handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			//scenery_component::handle scenery(mesh_component::handle, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			light_component::handle add_light(const noob::light&);
			reflection_component::handle add_reflection(const noob::reflection&);
			shader_component::handle add_shader(const noob::prepared_shaders::info&);
			
			// These cache the shape for reuse, as they are simple parametrics.
			// TODO: Since these are globals, make them static, and/or separate from the stage class.
		
			noob::shape_component::handle sphere(float r);
			noob::shape_component::handle box(float x, float y, float z);
			noob::shape_component::handle cylinder(float r, float h);
			noob::shape_component::handle cone(float r, float h);
			noob::shape_component::handle capsule(float r, float h);
			noob::shape_component::handle plane(const noob::vec3& normal, float offset);

			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively. Might provide a way to cache frequently-used ones if needed.
			noob::shape_component::handle hull(const std::vector<noob::vec3>& point);
			noob::shape_component::handle trimesh(const noob::basic_mesh& mesh);
			
			
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

