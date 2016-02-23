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

			static noob::shapes_holder::handle box(float x, float y, float z);

			static noob::shapes_holder::handle cylinder(float r, float h);

			static noob::shapes_holder::handle cone(float r, float h);

			static noob::shapes_holder::handle hull(const std::vector<noob::vec3>&, const std::string& name);

			static noob::shapes_holder::handle static_trimesh(const noob::basic_mesh&, const std::string& name);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			// If the name is the same as an existing model the new one replaces it, and everything should still work because the end-user only sees handles. :)
			static noob::basic_models_holder::handle basic_model(const noob::basic_mesh&, const std::string& name);

			// Loads a serialized model (from cereal binary)
			// TODO: Expand all such functions to load from cereal binary (and soon also sqlite)
			static noob::animated_models_holder::handle animated_model(const std::string& filename);

			// Skeletal animations (encompassing basic, single-bone animation...)
			static noob::skeletal_anims_holder::handle skeleton(const std::string& filename);

			// Lighting functions
			static void set_light(const noob::light&, const std::string&);
			static noob::light get_light(const std::string&);

			// Surface reflectivity
			static void set_reflection(const noob::reflection&, const std::string&);
			static noob::reflection get_reflection(const std::string&);			
			
			// These were needed in order to make graphics work with AngelScript. Seriously, why use variants for this anymore? Oh yeah... Variants let us use a single container type.
			static noob::shaders_holder::handle set_shader(const noob::basic_renderer::uniform&, const std::string& name);
			static noob::shaders_holder::handle set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name);

			// Utilities:
			// static noob::basic_mesh mesh_from_shape(const noob::shapes_holder::handle);
			
			struct model_and_scale
			{
				model_and_scale() : scales(noob::vec3(1.0, 1.0, 1.0)) {}
				noob::basic_models_holder::handle model_h;
				noob::vec3 scales;
			};

			static model_and_scale model_by_shape(const noob::shapes_holder::handle);

			static noob::shaders_holder::handle get_shader(const std::string& name);

			// The following are basic, commonly-used objects that we provide as a convenience.
			// noob::shape objects are a wrapper to Bullet shapes that provide a basic API to the rest of the app
			static noob::shapes_holder::handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;
			
			// These represent models in the graphics card buffer
			static noob::basic_models_holder::handle unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;
			
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
			static noob::shaders_holder::handle set_shader(const noob::prepared_shaders::uniform&, const std::string& name);

			static std::map<size_t, noob::basic_models_holder::handle> shapes_to_models;
			static std::unordered_map<std::string, noob::shapes_holder::handle> names_to_shapes;
			static std::unordered_map<std::string, noob::basic_models_holder::handle> names_to_basic_models;
			static std::unordered_map<std::string, noob::shaders_holder::handle> names_to_shaders;
			static std::unordered_map<std::string, noob::lights_holder::handle> names_to_lights;
			static std::unordered_map<std::string, noob::reflections_holder::handle> names_to_reflections;
			

	};
}
