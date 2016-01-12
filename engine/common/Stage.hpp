// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once


#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>

#include <boost/variant.hpp>

#include <btBulletDynamicsCommon.h>
#include <dcollide.h>

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
#include "Prop.hpp"
#include "Scenery.hpp"
#include "Body.hpp"
#include "Shape.hpp"
#include "Component.hpp"
#include "GlobalComponents.hpp"
// #include "IntrusiveBase.hpp"

#include <standalone/brigand.hpp>

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::body>> bodies_holder;


	class stage;

	class component_tag
	{
		friend class stage;

		public:
		es::storage::component_id get() const { return inner; }
		private:
		es::storage::component_id inner;
	};

	class stage
	{
		public:
			stage() : show_origin(true), view_mat(noob::identity_mat4()), projection_mat(noob::identity_mat4()), basic_lights( { noob::vec4(0.0, 1.0, 0.0, 0.1), noob::vec4(1.0, 0.0, 0.0, 0.1) } ) {}
			~stage();

			// This one must be called by the application. It really sucks but that's because the graphics API is (currently) static. This may well change soon enough.
			void init();

			// This one provides a way to bring everything back to scratch 
			void tear_down();

			// Call those every frame or so.
			void update(double dt);
			// TODO: Implement
			void draw(float window_width, float window_height) const;

			// Creates physics body. Those get made lots.
			noob::bodies_holder::handle body(noob::body_type, noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);
			unsigned int _body(noob::body_type, unsigned int, float, const noob::vec3&, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);

			// Parametric shapes. These get cached for reuse by the physics engine.
			noob::shapes_holder::handle sphere(float r);
			unsigned int _sphere(float r);
			
			noob::shapes_holder::handle box(float x, float y, float z);
			unsigned int _box(float x, float y, float z);

			noob::shapes_holder::handle cylinder(float r, float h);
			unsigned int _cylinder(float r, float h);
			
			noob::shapes_holder::handle cone(float r, float h);
			unsigned int _cone(float r, float h);

			// Point-based shapes
			noob::shapes_holder::handle hull(const std::vector<noob::vec3>&);
			unsigned int _hull(const std::vector<noob::vec3>&);

			noob::shapes_holder::handle static_trimesh(const noob::meshes_holder::handle);
			unsigned int _static_trimesh(unsigned int);
			
			// Adds a mesh to the scene.
			noob::meshes_holder::handle add_mesh(const noob::basic_mesh&);
			unsigned int _add_mesh(const noob::basic_mesh&);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			noob::basic_models_holder::handle basic_model(const noob::meshes_holder::handle);
			unsigned int _basic_model(unsigned int);

			// Loads a serialized model (from cereal binary)
			// TODO: Expand all such functions to load from cereal binary and also sqlite
			noob::animated_models_holder::handle animated_model(const std::string& filename);
			unsigned int _animated_model(const std::string&);

			// Skeletal animations (encompassing basic, single-bone animation...)
			noob::skeletal_anims_holder::handle skeleton(const std::string& filename);
			unsigned int _skeleton(const std::string&);

			// Lighting functions
			void set_light(const noob::light&, const std::string&);
			noob::light get_light(const std::string&);

			// Surface reflectivity
			void set_reflection(const noob::reflection&, const std::string&);
			noob::reflection get_reflection(const std::string&);			

			void set_shader(const noob::basic_renderer::uniform&, const std::string& name);
			void set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name);
			
			noob::shaders_holder::handle get_shader(const std::string& name);
			unsigned int _get_shader(const std::string&);

			// Tags that are used for faster access to our tag-tracker
			noob::component_tag scales_tag, mesh_tag, path_tag, shape_tag, shape_type_tag, body_tag, basic_model_tag, animated_model_tag, skeletal_anim_tag, shader_tag;

			// Indexable object tracking
			noob::meshes_holder meshes;
			noob::basic_models_holder basic_models;
			noob::animated_models_holder animated_models;
			//noob::collision_shapes_holder collision_shapes;
			noob::shapes_holder shapes;
			noob::bodies_holder bodies;
			noob::skeletal_anims_holder skeletal_anims;
			noob::lights_holder lights;
			noob::reflections_holder reflections;
			noob::shaders_holder shaders;

			// The following are basic, commonly-used objects that we class provide as a convenience.
			// noob::shape objects are a wrapper to Bullet shapes that provide a basic API to the rest of the app
			noob::shapes_holder::handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;
			
			unsigned int _unit_sphere_shape() const
			{
				return unit_sphere_shape.get_inner();
			}
			
			unsigned int _unit_cube_shape() const
			{
				return unit_cube_shape.get_inner();
			}

			unsigned int _unit_cylinder_shape() const
			{
				return unit_cylinder_shape.get_inner();
			}

			unsigned int _unit_cone_shape() const
			{
				return unit_cone_shape.get_inner();
			}

			// noob:basic_mesh objects are holders for an indexed trimesh
			noob::meshes_holder::handle unit_sphere_mesh, unit_cube_mesh, unit_capsule_mesh, unit_cylinder_mesh, unit_cone_mesh;
			
			unsigned int _unit_sphere_mesh() const
			{
				return unit_sphere_mesh.get_inner();
			}

			unsigned int _unit_cube_mesh() const
			{
				return unit_cube_mesh.get_inner();
			}

			unsigned int _unit_cylinder_mesh() const
			{
				return unit_cylinder_mesh.get_inner();
			}

			unsigned int _unit_cone_mesh() const
			{
				return unit_cone_mesh.get_inner();
			}

			// noob:basic_nodel objects like these represent models in the graphics card's buffer
			noob::basic_models_holder::handle unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;
			
			unsigned int _unit_sphere_model() const
			{
				return unit_sphere_model.get_inner();
			}

			unsigned int _unit_cube_model() const
			{
				return unit_cube_model.get_inner();
			}

			unsigned int _unit_cylinder_model() const
			{
				return unit_cylinder_model.get_inner();
			}

			unsigned int _unit_cone_model() const
			{
				return unit_cone_model.get_inner();
			}

			noob::shaders_holder::handle debug_shader, default_triplanar_shader, uv_shader;

			// Functions to create commonly-used configurations:
			// Actors are a ghost body with a skeletal model. They are controlled by custom logic in order to cut down on physics engine time
			es::entity actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const std::string& shading);
			unsigned int _actor(unsigned int _bod, unsigned int _model, const std::string& shading);

			// Props are simple rigid-body objects with leaned-down 3d models that cannot be animated via vertex weights. They also cannot apply movement to themselves.
			// Note: Bullet doesn't support movable trimeshes, so trimeshes passed into this function get implicitly turned into scenery.
			es::entity prop(const noob::bodies_holder::handle, const std::string& shading);
			unsigned int _prop(unsigned int _bod, const std::string& shading);

			//es::entity prop(const noob::bodies_holder::handle, const std::string& shading);
			es::entity prop(const noob::bodies_holder::handle, const noob::basic_models_holder::handle, const std::string& shading);
			unsigned int _prop(unsigned int _bod, unsigned int _model, const std::string& shading);

			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			es::entity scenery(const noob::meshes_holder::handle, const noob::vec3& pos, const std::string& shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));
			unsigned int _scenery(unsigned int _mesh, const noob::vec3& pos, const std::string& _shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));

			es::storage pool;

			// Utilities:
			noob::basic_mesh make_mesh(const noob::shapes_holder::handle);
			noob::basic_mesh _make_mesh(unsigned int _shape_h);

			// For parametrics, this one will return a normalized model with scaling coordinates. For triangles, scalings are <1, 1, 1>
			std::tuple<noob::basic_models_holder::handle, noob::vec3> get_model(const noob::shapes_holder::handle);
			std::tuple<unsigned int, noob::vec3> _get_model(unsigned int);

			bool show_origin;
			
			void set_basic_light(unsigned int, const noob::vec4&);
			noob::vec4 get_basic_light(unsigned int) const;

			noob::mat4 view_mat, projection_mat;
		
		protected:
			// template<typename T>
			// unsigned char register_es_component(T t, const std::string& friendly_name)
			// {
			// auto a (pool.register_component<T>(t, friendly_name));
			// return a;
			// }

	// Our component-entity system: The super-efficient handle-swapping fast-iterating dynamic magic enabler
			// Shader setting	
			void set_shader(const noob::prepared_shaders::uniform&, const std::string& name);

			std::array<noob::vec4, 2> basic_lights;

			noob::prepared_shaders renderer;

			dcollide::World ghost_world;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			
			// TODO: Test other data structures.
			// Benchmark, benchmark, benchmark!!! Multiplatform, too!!!
			std::map<float, shapes_holder::handle> sphere_shapes;
			std::map<std::tuple<float, float, float>, shapes_holder::handle> box_shapes;
			std::map<std::tuple<float, float>, shapes_holder::handle> cylinder_shapes;
			std::map<std::tuple<float, float>, shapes_holder::handle> cone_shapes;
			std::map<std::tuple<float, float>, shapes_holder::handle> capsule_shapes;
			// std::map<std::tuple<float,float,float,float>, shapes_holder::handle> planes;
			std::map<std::vector<std::array<float, 3>>, shapes_holder::handle> hull_shapes;
			std::map<std::vector<std::array<float, 3>>, shapes_holder::handle> trimesh_shapes;

			// std::map<float, basic_models_holder::handle> sphere_models;
			// std::map<std::tuple<float, float, float>, basic_models_holder::handle> box_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> cylinder_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> cone_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> capsule_models;
			// std::map<std::tuple<float,float,float,float>, basic_models_holder::handle> planes;
			// std::map<std::vector<std::array<float,3>>, basic_models_holder::handle> hull_models;
			// std::unordered_map<std::string, basic_models_holder::handle> trimesh_models;

			std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
			std::unordered_map<size_t, noob::meshes_holder::handle> shapes_to_meshes;
			std::unordered_map<size_t, noob::basic_models_holder::handle> meshes_to_models;
			std::unordered_map<size_t, noob::shapes_holder::handle> meshes_to_shapes;

			// std::unordered_map<size_t, noob::basic_models_holder::handle> models_for_parametrics;
			
			std::unordered_map<std::string, noob::shaders_holder::handle> names_to_shaders;
			std::unordered_map<std::string, noob::lights_holder::handle> names_to_lights;
			std::unordered_map<std::string, noob::reflections_holder::handle> names_to_reflections;
	};
}
