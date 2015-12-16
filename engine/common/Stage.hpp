// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once


#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>

#include <boost/variant.hpp>

#include <btBulletDynamicsCommon.h>
#include "es/storage.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "PreparedShaders.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "BodyController.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"
//#include "Body.hpp"
#include "Shape.hpp"
#include "Component.hpp"
// #include "IntrusiveBase.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_mesh>> meshes;
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_models;
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_models;
	typedef noob::component<std::unique_ptr<noob::shape>> shapes;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeletal_anims;
	typedef noob::component<std::unique_ptr<noob::body_controller>> bodies;
	typedef noob::component<noob::light> lights;
	typedef noob::component<noob::reflection> reflections;
	typedef noob::component<noob::prepared_shaders::info> shaders;

	class stage;

	class component_tag
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
			// This one must always be called before anything starts. Hard constraint.
			bool init();

			// This one provides a way to bring everything back to scratch without shutting down the main application.
			void tear_down();

			// Call those whever you need to.
			void update(double dt);
			void draw() const;

			// Creates physics body. Those get made lots.
			noob::bodies::handle body(noob::shapes::handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));

			// Parametric shapes. These get cached for reuse by the physics engine.
			noob::shapes::handle sphere(float r);
			noob::shapes::handle box(float x, float y, float z);
			noob::shapes::handle cylinder(float r, float h);
			noob::shapes::handle cone(float r, float h);
			noob::shapes::handle capsule(float r, float h);
			// noob::shapes::handle plane(const noob::vec3& normal, float offset);

			// Point-based shapes
			noob::shapes::handle hull(const std::vector<noob::vec3>&);
			noob::shapes::handle static_trimesh(const noob::meshes::handle);

			// Adds a mesh to the scene.
			noob::meshes::handle add_mesh(const noob::basic_mesh& h);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			// TODO: Provide mesh creation function from noob::shapes::handle
			noob::basic_models::handle basic_model(const noob::meshes::handle);

			// Loads a serialized model (from cereal binary)
			// TODO: Expand all such functions to load from cereal binary and also sqlite
			noob::animated_models::handle animated_model(const std::string& filename);

			// Skeletal animations (encompassing basic, single-bone animation...)
			noob::skeletal_anims::handle skeleton(const std::string& filename);

			// Lighting functions
			void set_light(const noob::light&, const std::string&);
			noob::light get_light(const std::string&);

			// Surface reflectivity
			void set_reflection(const noob::reflection&, const std::string&);
			noob::reflection get_reflection(const std::string&);			

			// Shader setting
			void set_shader(const noob::prepared_shaders::info&, const std::string& name);
			noob::prepared_shaders::info get_shader(const std::string& name);

			// Our component-entity system: The super-efficient handle-swapping fast-iterating dynamic magic enabler
			es::storage pool;

			// Tags that are used for faster access to our tag-tracker
			noob::component_tag mesh_tag, path_tag, shape_tag, shape_type_tag, body_tag, basic_model_tag, animated_model_tag, skeletal_anim_tag, basic_shader_tag, triplanar_shader_tag;

			// Indexable object tracking
			noob::meshes meshes_holder;
			noob::basic_models basic_models_holder;
			noob::animated_models animated_models_holder;
			noob::shapes shapes_holder;
			noob::bodies bodies_holder;
			noob::skeletal_anims skeletal_anims_holder;
			noob::lights lights_holder;
			noob::reflections reflections_holder;
			noob::shaders shaders_holder;

			// The following are basic, commonly-used objects that we class provide as a convenience.
			// noob::shape objects are a wrapper to Bullet shapes that provide a basic API to the rest of the app
			noob::shapes::handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;
			// noob:basic_mesh objects are holders for an indexed trimesh
			noob::meshes::handle unit_sphere_mesh, unit_cube_mesh, unit_capsule_mesh, unit_cylinder_mesh, unit_cone_mesh;
			// noob:basic_nodel objects like these represent models in the graphics card's buffer
			noob::basic_models::handle unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;

			// Functions to create commonly-used configurations:
			// Actors have character controllers and weighted models. They can be animated and can apply movement to themselves.
			es::entity actor(const noob::bodies::handle, const noob::animated_models::handle);
			// Props are simple rigid-body objects with leaned-down 3d models that cannot be animated via vertex weights. They also cannot apply movement to themselves.
			// Bullet doesn't support movable trimeshes, so trimeshes passed into this function get implicitly turned into scenery.
			// TODO: Switch to Newton?
			es::entity prop(const noob::bodies::handle);
			es::entity prop(const noob::bodies::handle, const noob::basic_models::handle);
			// Scenery is a non-movable item that is also made with a leaned-down mesh. Uses trimeshes as input. 
			es::entity scenery(const noob::meshes::handle, const noob::vec3& pos, const noob::versor& orient);


			// Utilities:
			noob::basic_mesh make_mesh(const noob::shapes::handle);
			// For parametrics, this one will return a normalized model that gets 
			std::tuple<noob::basic_models::handle,noob::vec3> get_model(const noob::shapes::handle);

		protected:

			// template<typename T>
			// unsigned char register_es_component(const std::string& friendly_name)
			// {
			// auto a (pool.register_component<T>(friendly_name));
			// return a;
			// }

			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// TODO: Test other data structures.
			// Benchmark, benchmark, benchmark!!! Multiplatform, too!!!
			std::map<float, shapes::handle> sphere_shapes;
			std::map<std::tuple<float, float, float>, shapes::handle> box_shapes;
			std::map<std::tuple<float, float>, shapes::handle> cylinder_shapes;
			std::map<std::tuple<float, float>, shapes::handle> cone_shapes;
			std::map<std::tuple<float, float>, shapes::handle> capsule_shapes;
			// std::map<std::tuple<float,float,float,float>, shapes::handle> planes;
			std::map<std::vector<std::array<float, 3>>, shapes::handle> hull_shapes;
			std::map<std::vector<std::array<float, 3>>, shapes::handle> trimesh_shapes;

			// std::map<float, basic_models::handle> sphere_models;
			// std::map<std::tuple<float, float, float>, basic_models::handle> box_models;
			// std::map<std::tuple<float, float>, basic_models::handle> cylinder_models;
			// std::map<std::tuple<float, float>, basic_models::handle> cone_models;
			// std::map<std::tuple<float, float>, basic_models::handle> capsule_models;
			// std::map<std::tuple<float,float,float,float>, basic_models::handle> planes;
			// std::map<std::vector<std::array<float,3>>, basic_models::handle> hull_models;
			// std::unordered_map<std::string, basic_models::handle> trimesh_models;

			std::unordered_map<size_t, noob::meshes::handle> shapes_to_meshes;
			std::unordered_map<size_t, noob::basic_models::handle> meshes_to_models;
			std::unordered_map<size_t, noob::shapes::handle> meshes_to_shapes;

			// std::unordered_map<size_t, noob::basic_models::handle> models_for_parametrics;
			
			std::unordered_map<std::string, noob::shaders::handle> shader_names;
			std::unordered_map<std::string, noob::lights::handle> light_names;
			std::unordered_map<std::string, noob::reflections::handle> reflection_names;
	};
}
