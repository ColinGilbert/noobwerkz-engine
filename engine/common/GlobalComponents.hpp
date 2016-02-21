// Stores singletons that should only be stored once each. This means:
// Shapes
// Meshes (TODO: Abolish)
// Stages
// Skeletal animations
// Mmodels

#pragma once

#include <memory>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "MeshUtils.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "Shape.hpp"
#include "Body.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "PreparedShaders.hpp"
#include "Component.hpp"

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::basic_mesh>> meshes_holder;
	typedef noob::component<std::unique_ptr<noob::basic_model>> basic_models_holder;
	typedef noob::component<std::unique_ptr<noob::animated_model>> animated_models_holder;
	typedef noob::component<std::unique_ptr<noob::shape>> shapes_holder;
	typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeletal_anims_holder;
	typedef noob::component<noob::light> lights_holder;
	typedef noob::component<noob::reflection> reflections_holder;
	typedef noob::component<noob::prepared_shaders::uniform> shaders_holder;


	class globals
	{
		public:
			// Provides sane defaults in order not to crash the app in case of erroneous access.
			static void init();

			// Parametric shapes. These get cached for reuse by the physics engine.
			static noob::shapes_holder::handle sphere(float r);
			// static unsigned int _sphere(float r);

			static noob::shapes_holder::handle box(float x, float y, float z);
			// static unsigned int _box(float x, float y, float z);

			static noob::shapes_holder::handle cylinder(float r, float h);
			// static unsigned int _cylinder(float r, float h);

			static noob::shapes_holder::handle cone(float r, float h);
			// static unsigned int _cone(float r, float h);

			// Point-based shapes
			static noob::shapes_holder::handle hull(const std::vector<noob::vec3>&);
			// static unsigned int _hull(const std::vector<noob::vec3>&);

			static noob::shapes_holder::handle static_trimesh(const noob::meshes_holder::handle);
			// static unsigned int _static_trimesh(unsigned int);

			// Adds a mesh to the scene.
			static noob::meshes_holder::handle add_mesh(const noob::basic_mesh&);
			// static unsigned int _add_mesh(const noob::basic_mesh&);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			static noob::basic_models_holder::handle basic_model(const noob::meshes_holder::handle);
			// static unsigned int _basic_model(unsigned int);

			// Loads a serialized model (from cereal binary)
			// TODO: Expand all such functions to load from cereal binary and also sqlite
			static noob::animated_models_holder::handle animated_model(const std::string& filename);
			// static unsigned int _animated_model(const std::string&);

			// Skeletal animations (encompassing basic, single-bone animation...)
			static noob::skeletal_anims_holder::handle skeleton(const std::string& filename);
			// static unsigned int _skeleton(const std::string&);

			// Lighting functions
			static void set_light(const noob::light&, const std::string&);
			static noob::light get_light(const std::string&);

			// Surface reflectivity
			static void set_reflection(const noob::reflection&, const std::string&);
			static noob::reflection get_reflection(const std::string&);			
			
			// These were needed in order to make graphics work with AngelScript. Seriously, why use variants for this anymore?
			static void set_shader(const noob::basic_renderer::uniform&, const std::string& name);
			static void set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name);

			// Utilities:
			static noob::basic_mesh make_mesh(const noob::shapes_holder::handle);
			// static noob::basic_mesh _make_mesh(unsigned int _shape_h);

			// For parametrics, this one will return a normalized model with scaling coordinates. For triangles, scalings are <1, 1, 1>
			static std::tuple<noob::basic_models_holder::handle, noob::vec3> get_model(const noob::shapes_holder::handle);
			static std::tuple<unsigned int, noob::vec3> _get_model(unsigned int);

			static noob::shaders_holder::handle get_shader(const std::string& name);
			// static unsigned int _get_shader(const std::string&);

			// The following are basic, commonly-used objects that we provide as a convenience.
			// noob::shape objects are a wrapper to Bullet shapes that provide a basic API to the rest of the app
			static noob::shapes_holder::handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;
/*
			static unsigned int _unit_sphere_shape() 
			{
				return unit_sphere_shape.get_inner();
			}

			static unsigned int _unit_cube_shape() 
			{
				return unit_cube_shape.get_inner();
			}

			static unsigned int _unit_cylinder_shape() 
			{
				return unit_cylinder_shape.get_inner();
			}

			static unsigned int _unit_cone_shape()
			{
				return unit_cone_shape.get_inner();
			}
*/
			// noob:basic_mesh objects are holders for an indexed trimesh
			static noob::meshes_holder::handle unit_sphere_mesh, unit_cube_mesh, unit_capsule_mesh, unit_cylinder_mesh, unit_cone_mesh;
/*
			static unsigned int _unit_sphere_mesh() 
			{
				return unit_sphere_mesh.get_inner();
			}

			static unsigned int _unit_cube_mesh() 
			{
				return unit_cube_mesh.get_inner();
			}

			static unsigned int _unit_cylinder_mesh() 
			{
				return unit_cylinder_mesh.get_inner();
			}

			static unsigned int _unit_cone_mesh() 
			{
				return unit_cone_mesh.get_inner();
			}
*/
			// noob:basic_nodel objects like these represent models in the graphics card's buffer
			static noob::basic_models_holder::handle unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;
/*
			static unsigned int _unit_sphere_model() 
			{
				return unit_sphere_model.get_inner();
			}

			static unsigned int _unit_cube_model() 
			{
				return unit_cube_model.get_inner();
			}

			static unsigned int _unit_cylinder_model() 
			{
				return unit_cylinder_model.get_inner();
			}

			static unsigned int _unit_cone_model() 
			{
				return unit_cone_model.get_inner();
			}
*/
			static noob::shaders_holder::handle debug_shader, default_triplanar_shader, uv_shader;

			static noob::prepared_shaders renderer;

			static meshes_holder meshes;
			static basic_models_holder basic_models;
			static animated_models_holder animated_models;
			static shapes_holder shapes;
			static skeletal_anims_holder skeletal_anims;
			static lights_holder lights;
			static reflections_holder reflections;
			static shaders_holder shaders;

		protected:

			static void set_shader(const noob::prepared_shaders::uniform&, const std::string& name);

			// TODO: Test other data structures.
			// Benchmark, benchmark, benchmark!!! Multiplatform, too!!!
			static std::map<float, shapes_holder::handle> sphere_shapes;
			static std::map<std::tuple<float, float, float>, shapes_holder::handle> box_shapes;
			static std::map<std::tuple<float, float>, shapes_holder::handle> cylinder_shapes;
			static std::map<std::tuple<float, float>, shapes_holder::handle> cone_shapes;
			static std::map<std::tuple<float, float>, shapes_holder::handle> capsule_shapes;
			// std::map<std::tuple<float,float,float,float>, shapes_holder::handle> planes;
			static std::map<std::vector<std::array<float, 3>>, shapes_holder::handle> hull_shapes;
			static std::map<std::vector<std::array<float, 3>>, shapes_holder::handle> trimesh_shapes;

			static std::unordered_map<size_t, noob::meshes_holder::handle> shapes_to_meshes;
			static std::unordered_map<size_t, noob::basic_models_holder::handle> meshes_to_models;
			static std::unordered_map<size_t, noob::shapes_holder::handle> meshes_to_shapes;

			static std::unordered_map<std::string, noob::shaders_holder::handle> names_to_shaders;
			static std::unordered_map<std::string, noob::lights_holder::handle> names_to_lights;
			static std::unordered_map<std::string, noob::reflections_holder::handle> names_to_reflections;

	};
}
