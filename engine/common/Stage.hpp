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
#include "Component.hpp"
#include "IntrusiveBase.hpp"


namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_models;	
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_models;
	typedef noob::component<std::unique_ptr<noob::shape>> shapes;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeletal_anims;
	typedef noob::component<noob::body> bodies;
	typedef noob::component<noob::light> lights;
	typedef noob::component<noob::reflection> reflections;
	typedef noob::component<noob::prepared_shaders::info> shaders;

	class stage;
	
	class es_wrapper
	{
		friend class stage;
		
		public:
			unsigned char get() const { return inner; }
		private:
			unsigned char inner;
	};

	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);

			void draw() const;
			
			noob::basic_models::handle basic_model(const noob::basic_mesh&);	
			noob::basic_models::handle basic_model(const shapes::handle&);
			
			// Loads a serialized model (from cereal binary)
			noob::animated_models::handle animated_model(const std::string& filename, const std::string& friendly_name);
			noob::skeletal_anims::handle skeleton(const std::string& filename, const std::string& friendly_name);

			// noob::actor actor(const noob::prop&, const animated_models::handle&, const std::string& friendly_name);
			// noob::prop prop(const noob::body&, const basic_models::handle&, const std::string& friendly_name);
			// noob::scenery scenery(const basic_models::handle&, const noob::vec3& pos, const noob::versor& orient, const std::string& friendly_name);
			
			noob::bodies::handle body(const noob::shapes::handle&, float mass, const noob::vec3& pos, const noob::versor& orient);
			noob::lights::handle light(const noob::light&);
			noob::reflections::handle reflection(const noob::reflection&);
			noob::shaders::handle shader(const noob::prepared_shaders::info&, const std::string& name);
			
			noob::shapes::handle sphere(float r);
			noob::shapes::handle box(float x, float y, float z);
			noob::shapes::handle cylinder(float r, float h);
			noob::shapes::handle cone(float r, float h);
			noob::shapes::handle capsule(float r, float h);
			noob::shapes::handle plane(const noob::vec3& normal, float offset);
			noob::shapes::handle hull(const std::vector<noob::vec3>& points, const std::string& name);
			noob::shapes::handle trimesh(const noob::basic_mesh& mesh, const std::string& name);

			es::storage pool;

			noob::es_wrapper path_tag, shape_tag, shape_type_tag, body_tag, movement_controller_tag, basic_model_tag, animated_model_tag, skeletal_anim_tag, basic_shader_tag, triplanar_shader_tag;

			noob::basic_models basic_models_holder;
			noob::animated_models animated_models_holder;
			noob::shapes shapes_holder;
			noob::bodies bodies_holder;
			noob::skeletal_anims skeletal_anims_holder;
			noob::lights lights_holder;
			noob::reflections reflections_holder;
			noob::shaders shaders_holder;

		protected:

			template<typename T>
			unsigned char register_es_component(const std::string& friendly_name)
			{
				auto a (pool.register_component<T>(friendly_name));
				return a;
			};

			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// TODO: Test other data structures.
			std::map<float, shapes::handle> spheres;
			std::map<std::tuple<float, float, float>, shapes::handle> boxes;
			std::map<std::tuple<float, float>, shapes::handle> cylinders;
			std::map<std::tuple<float, float>, shapes::handle> cones;
			std::map<std::tuple<float, float>, shapes::handle> capsules;
			std::map<std::tuple<float,float,float,float>, shapes::handle> planes;
			std::unordered_map<std::string, shapes::handle> hulls;
			std::unordered_map<std::string, shapes::handle> trimeshes;
	};
}
