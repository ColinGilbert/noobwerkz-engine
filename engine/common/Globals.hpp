// Stores objects that should be pooled. This means:
// Shapes
// Stages
// Skeletal animations
// 3D Models
// Audio samples

#pragma once

#include <memory>
#include <rdestl/hash_map.h>
#include <rdestl/sorted_vector.h>

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
#include "Component.hpp"
#include "Ghost.hpp"
#include "ComponentDefines.hpp"
#include "BasicRenderer.hpp"
#include "TriplanarGradientMap.hpp"
#include "AudioSample.hpp"
#include "SoundInterface.hpp"
#include "FastHashTable.hpp"
#include "Mixer.hpp"
#include "NoobCommon.hpp"

namespace noob
{
	
	class body_descriptor
	{
		friend class globals;
		public:
		bool is_physical() noexcept(true) { return physical; }
		protected:
		bool physical;
	};
	
	class globals
	{
		protected:
			static globals* ptr_to_instance;

			globals() noexcept(true) : sample_rate(44100), num_overflows(0) {}

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

			// Provides sane defaults in order not to crash the app in case of erroneous access.
			bool init() noexcept(true);

			// Parametric shapes. These get cached for reuse by the physics engine.
			noob::shape_handle sphere_shape(float r) noexcept(true);

			noob::shape_handle box_shape(float x, float y, float z) noexcept(true);

			noob::shape_handle cylinder_shape(float r, float h) noexcept(true);

			noob::shape_handle cone_shape(float r, float h) noexcept(true);

			noob::shape_handle hull_shape(const std::vector<noob::vec3>&, const std::string& name) noexcept(true);

			noob::shape_handle static_trimesh_shape(const noob::basic_mesh&, const std::string& name) noexcept(true);

			// Basic model creation. Those don't have bone weights built-in, so its lighter on the video card. Great for non-animated meshes and also scenery.
			// If the name is the same as an existing model the new one replaces it, and everything should still work because the end-user only sees handles and the engine handles it under-the-hood. :)
			noob::model_handle basic_model(const noob::basic_mesh&, const std::string& name) noexcept(true);

			noob::animated_model_handle animated_model(const std::string& filename) noexcept(true);

			noob::skeletal_anim_handle skeleton(const std::string& filename) noexcept(true);

			void set_shader(const noob::basic_renderer::uniform&, const std::string& name) noexcept(true);
			void set_shader(const noob::triplanar_gradient_map_renderer::uniform&, const std::string& name) noexcept(true);

			struct shader_results
			{
				shader_results() noexcept(true) : type(shader_type::BASIC), handle(0) {}
				bool operator==(const noob::globals::shader_results& other) const noexcept(true)
				{
					return (type == other.type && handle == other.handle);
				}
				shader_type type;
				size_t handle;
			};

			noob::globals::shader_results get_shader(const std::string& name) const noexcept(true);

			noob::light_handle set_light(const noob::light&, const std::string& name) noexcept(true);
			noob::light_handle get_light(const std::string& name) const noexcept(true);

			noob::reflectance_handle set_reflectance(const noob::reflectance&, const std::string& name) noexcept(true);
			noob::reflectance_handle get_reflectance(const std::string& name) const noexcept(true);
			// void set_point_light(const noob::point_light&, const std::string& name) noexcept(true);
			// noob::point_light_handle get_point_light(const std::string& name) noexcept(true);

			// void set_spotlight(const noob::spotlight&, const std::string& name) noexcept(true);
			// noob::spotlight_handle get_spotlight(const std::string& name) noexcept(true);

			// Those are easy to represent as a scaled item, and save a lot on the video card if repeated.
			scaled_model sphere_model(float r) noexcept(true);
			scaled_model box_model(float x, float y, float z) noexcept(true);
			scaled_model cylinder_model(float r, float h) noexcept(true);
			scaled_model cone_model(float r, float h) noexcept(true);
			// Those will create lots more data
			scaled_model model_from_mesh(const noob::basic_mesh&, const std::string& name) noexcept(true);
			// Causes crashes with scripts. Therefore, keep indoors.
			scaled_model model_by_shape(const noob::shape_handle) noexcept(true);


			//
			// Data members:
			//

			// The following are basic, commonly-used objects that we provide as a convenience.
			noob::shape_handle unit_sphere_shape, unit_cube_shape, unit_capsule_shape, unit_cylinder_shape, unit_cone_shape;

			// These represent models in the graphics card buffer
			noob::scaled_model unit_sphere_model, unit_cube_model, unit_capsule_model, unit_cylinder_model, unit_cone_model;

			noob::basic_shader_handle debug_shader;
			noob::triplanar_shader_handle default_triplanar_shader;

			noob::light_handle default_light;
			noob::reflectance_handle default_reflectance;

			basic_models_holder basic_models;
			animated_models_holder animated_models;
			shapes_holder shapes;
			skeletal_anims_holder skeletal_anims;
			lights_holder lights;
			reflectances_holder reflectances;
			basic_shaders_holder basic_shaders;
			triplanar_shaders_holder triplanar_shaders;
			samples_holder samples;
			noob::basic_renderer basic_drawer;
			noob::triplanar_gradient_map_renderer triplanar_drawer;

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

			noob::body_descriptor physical_body_descriptor, ghost_body_descriptor;


			noob::mixer master_mixer;
			noob::sound_interface audio_interface;
			size_t sample_rate;
			// While resampling data, some samples may suffer from overflow of a few values.
			// This catches them so that they may be used in the next audio callback.
			rde::vector<double> resample_overflow;
			uint32_t num_overflows;
		
		protected:


			shape_handle add_shape(const noob::shape& s)
			{
				noob::shape temp = s;
				noob::shape_handle h = shapes.add(s);
				temp.index = h.get_inner();
				shapes.set(h, temp);
				return h;
			}

			noob::fast_hashtable shapes_to_models;
			rde::sorted_vector<float,size_t> spheres;
			std::unordered_map<std::string, noob::shape_handle> names_to_shapes;
			std::unordered_map<std::string, noob::model_handle> names_to_basic_models;
			std::unordered_map<std::string, noob::globals::shader_results> names_to_shaders;
			std::unordered_map<std::string, noob::light_handle> names_to_lights;
			std::unordered_map<std::string, noob::reflectance_handle> names_to_reflectances;
	};
}
