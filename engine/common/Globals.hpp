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
#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/math/math_funcs.hpp>

#include "Clock.hpp"
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
#include "ProfilingInfo.hpp"

// TODO: Remove this horror.

namespace noob
{
	class globals
	{
		friend class sound_interface;
		public:

		static constexpr uint32_t num_pseudo_randoms = 256;

		// Provides sane defaults in order not to crash the app in case of erroneous access.
		bool init() noexcept(true);

		// Parametric shapes. These get cached for reuse by the physics engine.
		noob::shape_handle sphere_shape(float r) noexcept(true);

		noob::shape_handle box_shape(float x, float y, float z) noexcept(true);

		noob::shape_handle hull_shape(const std::vector<noob::vec3f>&) noexcept(true);

		noob::shape_handle static_trimesh_shape(const noob::mesh_3d&) noexcept(true);

		noob::skeletal_anim_handle skeleton(const std::string& filename) noexcept(true);

		noob::point_light_handle set_point_light(const noob::point_light&, const std::string& name) noexcept(true);
		noob::point_light_handle get_point_light(const std::string& name) const noexcept(true);

		noob::reflectance_handle set_reflectance(const noob::reflectance&, const std::string& name) noexcept(true);
		noob::reflectance_handle get_reflectance(const std::string& name) const noexcept(true);

		noob::shape shape_from_handle(const noob::shape_handle) const noexcept(true);
		noob::instanced_model_handle model_from_shape(noob::shape_handle, uint32_t) noexcept(true);
		
		double get_random() noexcept(true);

		size_t get_sample_rate() const noexcept(true)
		{
			return sample_rate;
		}

		// ---------------
		// Data members:
		// ---------------
		
		noob::profiler_snap profile_run;
		
		shapes_holder shapes;
		skeletal_anims_holder skeletal_anims;
		point_lights_holder lights;
		reflectances_holder reflectances;
		samples_holder samples;

		noob::mixer master_mixer;

		// The following are basic, commonly-used objects that we provide as a convenience.
		noob::shape_handle get_unit_sphere_shape() const noexcept(true)
		{
			return unit_sphere_shape;
		}

		noob::shape_handle get_unit_cube_shape() const noexcept(true)
		{
			return unit_cube_shape;
		}
		
		noob::point_light_handle get_default_light() const noexcept(true)
		{
			return default_light;
		}
		
		size_t sample_rate;

		bool finished_init() const noexcept(true)
		{
			return init_done;
		}

		protected:
		// Hack used to set the shape's index-to-self
		shape_handle add_shape(const noob::shape& s) noexcept(true);

		// The following are basic, commonly-used objects that we provide as a convenience.
		noob::shape_handle unit_sphere_shape, unit_cube_shape;
		noob::point_light_handle default_light;
		noob::reflectance_handle default_reflectance;

		noob::fast_hashtable shapes_to_models;

		uint32_t current_random;
		rde::fixed_array<double, num_pseudo_randoms> pseudo_randoms;

		rde::hash_map<rde::string, noob::shape_handle> names_to_shapes;
		rde::hash_map<rde::string, noob::point_light_handle> names_to_lights;
		rde::hash_map<rde::string, noob::reflectance_handle> names_to_reflectances;

		std::atomic<bool> init_done;
	};

	static noob::singleton<noob::globals> globals_instance;
	static noob::globals& get_globals() { return globals_instance.get(); }
}
