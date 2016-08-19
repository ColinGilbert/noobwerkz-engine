#pragma once

#include <rdestl/fixed_array.h>

#include "ComponentDefines.hpp"
#include "Component.hpp"
#include "MathFuncs.hpp"
#include "Timing.hpp"
#include "RandomGenerator.hpp"


namespace noob
{
	// These are linked to ghosts onstage, and are invalidated on either first contact with anything that isn't another particle, or on running out of time.
	struct particle
	{
		particle() noexcept(true) : active(false), time_left(0.0), colour(noob::vec4(1.0, 0.0, 1.0, 1.0)) {}
		bool active;
		float time_left;
		noob::vec3 velocity;
		noob::ghost_handle ghost;
		noob::vec4 colour;
	};

	struct particle_system_descriptor
	{
		uint32_t emits_per_second;

		float lifespan_base, lifespan_variance, damping, gravity_multiplier;

		noob::vec3 center, emit_direction, emit_direction_variance, wind;

		noob::reflectance_handle particle_reflectance;

		noob::shape_handle particle_shape;
	};

	class particle_system
	{
		friend class stage;
		public:

		particle_system() noexcept(true) : active(false), first_free(0), emits_per_second(0), nanos_until_emit(0), lifespan_base(5.0), lifespan_variance(0.2), damping(0.995), gravity_multiplier(0.05), wind_multiplier(1.0), emit_force(10.0), center(noob::vec3(0.0, 0.0, 0.0)), emit_direction(noob::vec3(0.0, 1.0, 0.0)), emit_direction_variance(noob::vec3(0.2, 0.0, 0.2)), wind(noob::vec3(0.2, 0.0, -0.3)) {}

		static constexpr uint32_t max_particles = 64;
		
		void set_particle_lifespan(noob::duration d, float variance) noexcept(true)
		{
			lifespan_base = static_cast<float>(d.count());
			lifespan_variance = variance;
		}

		void set_emits_per_sec(uint32_t num_emits) noexcept(true)
		{
			emits_per_second = num_emits;
			nanos_between_emits = noob::billion / num_emits;
			nanos_until_emit = nanos_between_emits;
		}

		uint32_t get_next_free(uint32_t from) const noexcept(true)
		{
			for (uint32_t i = from; i < max_particles; ++i)
			{
				if (particles[i].active) return i;
			}
			for (uint32_t i = 0; i < from; ++i)
			{
				if (particles[i].active) return i;
			}
			return std::numeric_limits<uint32_t>::max();
		}

		
		bool active;

		protected:


		uint32_t first_free, emits_per_second;

		uint64_t nanos_until_emit, nanos_between_emits;

		float lifespan_base, lifespan_variance, damping, gravity_multiplier, wind_multiplier, emit_force;

		noob::vec3 center, emit_direction, emit_direction_variance, wind;

		noob::time last_update_time, current_update_time;

		rde::fixed_array<particle, max_particles> particles;

		noob::shape_handle particle_shape;

		noob::reflectance_handle particle_reflectance;
	};

	// typedef noob::component<noob::particle_system> particle_systems_holder;
	typedef noob::handle<noob::particle_system> particle_system_handle;
	typedef noob::handle<noob::particle> particle_handle;
}
