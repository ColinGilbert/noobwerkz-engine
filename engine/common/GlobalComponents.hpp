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
	typedef noob::component<noob::reflectance> reflectances_holder;
	// typedef noob::component<noob::directional_light> directional_lights_holder;
	// typedef noob::component<noob::point_light> point_lights_holder;
	// typedef noob::component<noob::spotlight> spotlights_holder;
	
	typedef noob::component<noob::prepared_shaders::uniform> shaders_holder;


	struct scaled_model
	{
		scaled_model() : scales(noob::vec3(1.0, 1.0, 1.0)) {}
		noob::basic_models_holder::handle model_h;
		noob::reflectances_holder::handle reflect_h;
		noob::vec3 scales;
	};

	
	class globals
	{
		public:
			// Provides sane defaults in order not to crash the app in case of erroneous access.
			bool init();

			// Parametric shapes. These get cached for reuse by the physics engine.
			noob::shapes_holder::handle sphere_shape(float r);

			noob::shapes_holder::handle box_shape(float x, float y, float z);

			noob::shapes_holder::handle cylinder_shape(float r, float h);

			noob::shapes_holder::handle cone_shape(float r, float h);

			noob::shapes_holder::handle hull_shape(const std::vector<noob::vec3>&, const std::string& name);

			noob::shapes_holder::handle static_trimesh_shape(const noob::basic_mesh&, const std::string& name);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			// If the name is the same as an existing model the new one replaces it, and everything should still work because the end-user only sees handles. :)
			noob::basic_models_holder::handle basic_model(const noob::basic_mesh&, const std::string& name);

			// Loads a serialized model (from cereal binary)
			// TODO: Expand all such functions to load from cereal binary (and soon also sqlite)
			noob::animated_models_holder::handle animated_model(const std::string& filename);

			// Skeletal animations (encompassing basic, single-bone animation...)
			noob::skeletal_anims_holder::handle skeleton(const std::string& filename);

			// Lighting functions
			// noob::lights_holder::handle set_light(const noob::light&, const std::string&);
			// noob::lights_holder::handle get_light_handle(const std::string&) const;
			// noob::light get_light(const noob::lights_holder::handle) const;

			// Surface reflectivity
			// void set_reflection(const noob::reflection&, const std::string&);
			// noob::reflection get_reflection(const std::string&);			

			// These were needed in order to make graphics work with AngelScript. Seriously, why use variants for this anymore? Oh yeah... Variants let us use a single container type.
			void set_shader(const noob::basic_renderer::uniform&, const std::string& name);
			void set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name);
			void set_shader(const noob::triplanar_gradient_map_renderer_lit::uniform&, const std::string& name);
			
			noob::shaders_holder::handle get_shader(const std::string& name) const;

			// Utilities:
			noob::lights_holder::handle set_light(const noob::light&, const std::string& name);
			noob::lights_holder::handle get_light(const std::string& name) const;

			
			noob::reflectances_holder::handle set_reflectance(const noob::reflectance&, const std::string& name);
			noob::reflectances_holder::handle get_reflectance(const std::string& name) const;
			// void set_point_light(const noob::point_light&, const std::string& name);
			// noob::point_lights_holder::handle get_point_light(const std::string& name);

			// void set_spotlight(const noob::spotlight&, const std::string& name);
			// noob::spotlights_holder::handle get_spotlight(const std::string& name);

			// Those are easy to represent as a scaled item, and save a lot on the video card if repeated.
			scaled_model sphere_model(float r);
			scaled_model box_model(float x, float y, float z);
			scaled_model cylinder_model(float r, float h);
			scaled_model cone_model(float r, float h);
			// Those will create lots more data
			scaled_model model_from_mesh(const noob::basic_mesh&, const std::string& name);
			// Causes crashes with scripts. Therefore, keep indoors.
			scaled_model model_by_shape(const noob::shapes_holder::handle);

			// The following are basic, commonly-used objects that we provide as a convenience.
			// noob::shape objects are a wrapper to Bullet shapes that provide a basic API to the rest of the app
			noob::shapes_holder::handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;

			// These represent models in the graphics card buffer
			noob::scaled_model unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;

			noob::shaders_holder::handle debug_shader, default_triplanar_shader, uv_shader;

			noob::lights_holder::handle default_light;
			noob::reflectances_holder::handle default_reflectance;

			noob::prepared_shaders renderer;

			meshes_holder meshes;
			basic_models_holder basic_models;
			animated_models_holder animated_models;
			shapes_holder shapes;
			skeletal_anims_holder skeletal_anims;
			lights_holder lights;
			reflectances_holder reflectances;
			shaders_holder shaders;
			// directional_lights_holder directional_lights;
			// point_lights_holder point_lights;
			// spotlights_holder spotlights;

		protected:
			void set_shader(const noob::prepared_shaders::uniform&, const std::string& name);

			std::map<size_t, noob::basic_models_holder::handle> shapes_to_models;
			std::unordered_map<std::string, noob::shapes_holder::handle> names_to_shapes;
			std::unordered_map<std::string, noob::basic_models_holder::handle> names_to_basic_models;
			std::unordered_map<std::string, noob::shaders_holder::handle> names_to_shaders;
			std::unordered_map<std::string, noob::lights_holder::handle> names_to_lights;
			std::unordered_map<std::string, noob::reflectances_holder::handle> names_to_reflectances;


	};
}
