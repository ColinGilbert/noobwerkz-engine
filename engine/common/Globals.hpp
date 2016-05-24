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
#include "ScaledModel.hpp"
#include "AnimatedModel.hpp"
#include "Shape.hpp"
#include "Body.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "ShaderUniforms.hpp"
#include "Component.hpp"
#include "Ghost.hpp"
#include "ComponentDefines.hpp"


namespace noob
{
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

			void set_shader(const noob::basic_shader_uniform&, const std::string& name);
			void set_shader(const noob::triplanar_shader_uniform&, const std::string& name);
			
			struct shader_results
			{
				shader_type type;
				size_t handle_num;
			};

			shader_results get_shader(const std::string& name) const;

			// Utilities:
			noob::light_handle set_light(const noob::light&, const std::string& name);
			noob::light_handle get_light(const std::string& name) const;

			
			noob::reflectance_handle set_reflectance(const noob::reflectance&, const std::string& name);
			noob::reflectance_handle get_reflectance(const std::string& name) const;
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

			// noob::shaders_holder::handle debug_shader, default_triplanar_shader, uv_shader;
			
			noob::basic_shader_handle debug_shader;
			noob::triplanar_shader_handle default_triplanar_shader;

			noob::light_handle default_light;
			noob::reflectance_handle default_reflectance;

			// noob::prepared_shaders renderer;

			meshes_holder meshes;
			basic_models_holder basic_models;
			animated_models_holder animated_models;
			shapes_holder shapes;
			skeletal_anims_holder skeletal_anims;
			lights_holder lights;
			reflectances_holder reflectances;
			basic_shaders_holder basic_shaders;
			triplanar_shaders_holder triplanar_shaders;

			// directional_lights_holder directional_lights;
			// point_lights_holder point_lights;
			// spotlights_holder spotlights;

			enum collision_type
			{
				NOTHING = 0,
				TERRAIN = 1 << 1,
				CHARACTER = 1 << 2,
				TEAM_A = 1 << 3,
				TEAM_B = 1 << 4,
				POWERUP = 1 << 5,
			};

		protected:
			// void set_shader(const noob::prepared_shaders::uniform&, const std::string& name);

			std::map<size_t, noob::model_handle> shapes_to_models;
			std::unordered_map<std::string, noob::shape_handle> names_to_shapes;
			std::unordered_map<std::string, noob::model_handle> names_to_basic_models;
			std::unordered_map<std::string, std::tuple<uint8_t, size_t>> names_to_shaders;
			std::unordered_map<std::string, noob::light_handle> names_to_lights;
			std::unordered_map<std::string, noob::reflectance_handle> names_to_reflectances;
	};
}
