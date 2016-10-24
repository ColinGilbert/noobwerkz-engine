// Stores objects that should be pooled. This means:
// Shapes
// 3D Models
// Textures, shader uniforms
// Armatures and skeletal animations
// Actor configurations
// Audio samples
// Stages (potentially)

#pragma once

#include <memory>
#include <atomic>

#include <rdestl/rde_string.h>
#include <rdestl/sort.h>
#include <rdestl/hash_map.h>

// #include <noob/component/component.hpp>
#include <noob/fast_hashtable/fast_hashtable.hpp>

#include "NoobDefines.hpp"
#include "Clock.hpp"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "MeshUtils.hpp"
#include "Shape.hpp"
#include "Body.hpp"
#include "SkeletalAnim.hpp"
#include "Light.hpp"
#include "Ghost.hpp"
#include "AudioSample.hpp"
#include "SoundInterface.hpp"

#include "Mixer.hpp"
#include "Actor.hpp"
#include "ShaderVariant.hpp"
#include "ProfilingInfo.hpp"



// Globals: Currently holds classes requiring data held in common storage.
// TODO: Implement singleton class template.
// TODO: Separate into different classes.
// TODO: Make immutable upon assignment.
// TODO: Allocate storage ahead-of-time.
// TODO: Get a better allocator.

namespace noob
{
	class globals
	{
		friend class sound_interface;

		protected:
		static globals* ptr_to_instance;

		globals() noexcept(true) : sample_rate(44100), init_done(false) {}

		globals(const globals& rhs) noexcept(true)
		{
			ptr_to_instance = rhs.ptr_to_instance;
		}

		globals& operator=(const globals& rhs) noexcept(true)
		{
			if (this != &rhs)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}
			return *this;
		}

		~globals() noexcept(true) {}
		public:
		static globals& get_instance() noexcept(true)
		{
			static globals the_instance;
			ptr_to_instance = &the_instance;

			return *ptr_to_instance;
		}

		static constexpr uint32_t num_pseudo_randoms = 256;

		// Provides sane defaults in order not to crash the app in case of erroneous access.
		bool init() noexcept(true);

		// Parametric shapes. These get cached for reuse by the physics engine.
		noob::shape_handle sphere_shape(float r) noexcept(true);

		noob::shape_handle box_shape(float x, float y, float z) noexcept(true);

		// noob::shape_handle cylinder_shape(float r, float h) noexcept(true);

		// noob::shape_handle cone_shape(float r, float h) noexcept(true);

		noob::shape_handle hull_shape(const std::vector<noob::vec3>&) noexcept(true);

		noob::shape_handle static_trimesh_shape(const noob::basic_mesh&) noexcept(true);

		// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
		// If the name is the same as an existing model the new one replaces it, and everything should still work because the end-user only sees handles and the engine handles it under-the-hood. :)
		// noob::graphics::model_handle basic_model(const noob::basic_mesh&, const std::string& name) noexcept(true);
		// noob::animated_model_handle animated_model(const std::string& filename) noexcept(true);

		noob::skeletal_anim_handle skeleton(const std::string& filename) noexcept(true);

		// void set_shader(const noob::basic_renderer::uniform&, const std::string& name) noexcept(true);
		// void set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name) noexcept(true);

		noob::shader_variant get_shader(const std::string& name) const noexcept(true);

		noob::light_handle set_light(const noob::light&, const std::string& name) noexcept(true);
		noob::light_handle get_light(const std::string& name) const noexcept(true);

		noob::reflectance_handle set_reflectance(const noob::reflectance&, const std::string& name) noexcept(true);
		noob::reflectance_handle get_reflectance(const std::string& name) const noexcept(true);

		void set_actor_blueprints(const noob::actor_blueprints&, const std::string& name) noexcept(true);
		noob::actor_blueprints_handle get_actor_blueprints(const std::string& name) const noexcept(true);

		// Those are easy to represent as a scaled item, and save a lot on the video card if repeated.
		std::tuple<noob::model, noob::vec3> sphere_model(float r) noexcept(true);
		std::tuple<noob::model, noob::vec3> box_model(float x, float y, float z) noexcept(true);
		// std::tuple<noob::model, noob::vec3> cylinder_model(float r, float h) noexcept(true);
		// std::tuple<noob::model, noob::vec3> cone_model(float r, float h) noexcept(true);

		// std::tuple<noob::model, bool, noob::vec3> model_from_mesh(const noob::basic_mesh&) noexcept(true);
		// std::tuple<noob::graphics::model_instanced model_from_shape(const noob::shape_handle) noexcept(true);

		double get_random() noexcept(true);


		size_t get_sample_rate() const noexcept(true)
		{
			return sample_rate;
		}
		// ---------------
		// Data members:
		// ---------------

		noob::profiler_snap profile_run;


		// basic_models_holder basic_models;
		// animated_models_holder animated_models;
		shapes_holder shapes;
		skeletal_anims_holder skeletal_anims;
		lights_holder lights;
		reflectances_holder reflectances;
		// basic_shaders_holder basic_shaders;
		// triplanar_shaders_holder triplanar_shaders;
		shader_variants_holder shaders_variants;
		samples_holder samples;
		actor_blueprints_holder actor_blueprints;
		strings_holder strings;

		// noob::basic_renderer basic_drawer;
		// noob::triplanar_gradient_map_renderer triplanar_drawer;

		noob::mixer master_mixer;
		noob::sound_interface audio_interface;


		// The following are basic, commonly-used objects that we provide as a convenience.
		noob::shape_handle get_unit_sphere_shape() const noexcept(true)
		{
			return unit_sphere_shape;
		}

		noob::shape_handle get_unit_cube_shape() const noexcept(true)
		{
			return unit_cube_shape;
		}
		/*
		   noob::shape_handle get_unit_capsule_shape() const noexcept(true)
		   {
		   return unit_capsule_shape;
		   }

		   noob::shape_handle get_unit_cylinder_shape() const noexcept(true)
		   {
		   return unit_cylinder_shape;
		   }

		   noob::shape_handle get_unit_cone_shape() const noexcept(true)
		   {
		   return unit_cone_shape;
		   }
		   */
		// These represent models in the graphics card buffer

		noob::model get_unit_sphere_model() const noexcept(true)
		{
			return unit_sphere_model;
		}

		noob::model get_unit_cube_model() const noexcept(true)
		{
			return unit_cube_model;
		}
		/*			
					std::tuple<noob::model, bool, noob::vec3> get_unit_capsule_model() const noexcept(true)
					{
					return unit_cylinder_model;
					}

					std::tuple<noob::model, bool, noob::vec3> get_unit_cylinder_model() const noexcept(true)
					{
					return unit_cylinder_model;
					}

					std::tuple<noob::model, bool, noob::vec3> get_unit_cone_model() const noexcept(true)
					{
					return unit_cone_model;
					}


					noob::basic_shader_handle get_debug_shader() const noexcept(true)
					{
					return debug_shader;
					}

					noob::triplanar_shader_handle get_default_triplanar_shader() const noexcept(true)
					{
					return default_triplanar_shader;
					}
					*/
		noob::light_handle get_default_light() const noexcept(true)
		{
			return default_light;
		}

		noob::reflectance_handle get_default_reflectance() const noexcept(true)
		{
			return default_reflectance;
		}

		// sound_interface needs to set it and friend'ing it would allow it access to literally everything. However, once its completely stabilized we may do so again.	
		size_t sample_rate;


		bool finished_init() const noexcept(true)
		{
			return init_done;
		}

		protected:
		// Hack used to set the shape's index-to-self
		shape_handle add_shape(const noob::shape& s) noexcept(true);

		// The following are basic, commonly-used objects that we provide as a convenience.
		noob::shape_handle unit_sphere_shape, unit_cube_shape;//, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;

		// These represent models in the graphics card buffer
		noob::model unit_sphere_model, unit_cube_model;//, unit_capsule_model, unit_cylinder_model, unit_cone_model;

		noob::light_handle default_light;
		noob::reflectance_handle default_reflectance;

		noob::fast_hashtable shapes_to_models;

		uint32_t current_random;
		rde::fixed_array<double, num_pseudo_randoms> pseudo_randoms;


		rde::hash_map<rde::string, noob::shape_handle> names_to_shapes;
		// rde::hash_map<rde::string, noob::graphics::model_handle> names_to_basic_models;
		rde::hash_map<rde::string, noob::shader_variant> names_to_shaders;
		rde::hash_map<rde::string, noob::light_handle> names_to_lights;
		rde::hash_map<rde::string, noob::reflectance_handle> names_to_reflectances;
		rde::hash_map<rde::string, noob::actor_blueprints_handle> names_to_actor_blueprints;

		std::atomic<bool> init_done;
	};
}
