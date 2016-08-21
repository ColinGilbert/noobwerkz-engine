// In this engine, particles are linked to the simulation; they are ghost objects and are set to pop upon contact with physical objects (in reality they get reused.)
// Particles are also affected by gravity to a user-programmable degree.
// Particles don't collide with other particles of their own emitter.

// This class can only provide data members, as the stage is responsible for ghosts and drawing.
#pragma once

#include <rdestl/fixed_array.h>

#include "ComponentDefines.hpp"
#include "Component.hpp"
#include "MathFuncs.hpp"
#include "Timing.hpp"
//#include "RandomGenerator.hpp"


namespace noob
{
	// These are linked to ghosts onstage, and are invalidated on either first contact with anything that isn't another particle, or on running out of time.
	struct particle
	{
		particle() noexcept(true) : active(false), lifetime(noob::duration(0)), position(noob::vec3(0.0, 0.0, 0.0)), velocity(noob::vec3(0.0, 0.0, 0.0)), colour(noob::vec4(1.0, 1.0, 1.0, 0.0)) {}
		bool active;
		noob::duration lifetime;
		noob::vec3 position, velocity;
		noob::vec4 colour;
	};

	class particle_system
	{
		friend class stage;
		public:

		particle_system() noexcept(true) : active(false), first_free(0), emits_per_second(10), lifespan(5000000000), nanos_accum(0), damping(0.995), gravity_multiplier(0.05), emit_force(10.0), center(noob::vec3(0.0, 0.0, 0.0)), emit_direction(noob::vec3(0.0, 1.0, 0.0)), emit_direction_variance(noob::vec3(0.2, 0.2, 0.2)), wind(noob::vec3(0.2, 0.0, -0.3)) {}

		static constexpr uint32_t max_particles = 64;
		static constexpr uint32_t max_colours = 4;

		struct descriptor
		{
			uint32_t emits_per_second;

			uint64_t lifespan;

			float damping, gravity_multiplier, emit_force;

			noob::vec3 center, emit_direction, emit_direction_variance, wind;

			noob::reflectance_handle reflect;

			noob::shape_handle shape;

			rde::fixed_array<noob::vec4, noob::particle_system::max_colours> colours;

			void set_colour(uint32_t i, const noob::vec4& c)
			{
				if (i < noob::particle_system::max_colours)
				{
					colours[i] = c;
				}
				else
				{
					colours[noob::particle_system::max_colours - 1] = c;
				}
			}

			noob::vec4 get_colour(uint32_t i) const
			{
				if (i < noob::particle_system::max_colours)
				{
					return colours[i];
				}
				return colours[noob::particle_system::max_colours - 1];
			}
		};


		void set_properties(const noob::particle_system::descriptor& desc) noexcept(true)
		{
			emits_per_second = desc.emits_per_second;
			lifespan = desc.lifespan;
			damping = desc.damping;
			gravity_multiplier = desc.gravity_multiplier;
			emit_force = desc.emit_force;
			center = desc.center;
			emit_direction = desc.emit_direction;
			emit_direction_variance = desc.emit_direction_variance;
			wind = desc.wind;
			reflect = desc.reflect;
			shape = desc.shape;
		}

		noob::particle_system::descriptor get_properties() const noexcept(true)
		{
			noob::particle_system::descriptor desc;

			desc.emits_per_second = emits_per_second;
			desc.lifespan = lifespan;
			desc.damping = damping;
			desc.gravity_multiplier = gravity_multiplier;
			desc.emit_force = emit_force;
			desc.center = center;
			desc.emit_direction = emit_direction;
			desc.emit_direction_variance = emit_direction_variance;
			desc.wind = wind;
			desc.reflect = reflect;
			desc.shape = shape;

			return desc;
		}

		void set_particle_lifetime(uint64_t length_arg) noexcept(true)
		{
			lifespan = length_arg;
			one_colour_in_lifetime = lifespan / noob::particle_system::max_colours;
		}

		void set_emits_per_sec(uint32_t num_emits) noexcept(true)
		{
			emits_per_second = num_emits;
			nanos_between_emits = noob::billion / num_emits;
			nanos_accum = 0;
		}

		uint32_t get_next_free(uint32_t from) const noexcept(true)
		{
			for (uint32_t i = from; i < max_particles; ++i)
			{
				if (!particles[i].active) return i;
			}
			for (uint32_t i = 0; i < from; ++i)
			{
				if (!particles[i].active) return i;
			}
			return std::numeric_limits<uint32_t>::max();
		}

		// Could have been a get_colour(particle p), but it gets called lots and fewer arguments is better...
		noob::vec4 get_colour_from_particle_life(const noob::duration d) const noexcept(true)
		{
			/*			for (uint32_t i = 1; i < noob::particle_system::max_colours; ++i)
						{
						const float next_colour_transition = one_colour_in_lifetime * i;
						if (lifetime < next_colour_transition)
						{
						const float previous_colour_gradient = i * next_colour_transition;
						const float next_colour_gradient = i * next_colour_transition;
						const float distance_from_previous = lifetime - previous_colour_gradient;
						const float distance_to_next = next_colour_gradient - distance_from_previous;
						const noob::vec4 prev = colours[i - 1];
						const noob::vec4 next = colours[i];
						const noob::vec4 component_prev = prev * distance_to_next;
						const noob::vec4 component_next = next * distance_from_previous;
						noob::vec4 final_blend = component_prev;
						final_blend += component_next;
						return final_blend;
			//return (noob::vec4(prev[0] * distance_to_next, prev[1] * distance_to_next, prev[2] * distance_to_next, prev[3] * distance_to_next) + noob::vec4(next[0] * distance_from_previous, next[1] * distance_from_previous, next[2] * distance_from_previous, next[3] * distance_from_previous));
			}
			}
			*/



			logger::log("[ParticleSystem: get_colour_from_particle_life - Reached post-loop state!");

			return noob::vec4(1.0, 0.3, 1.0, 1.0);//colours[noob::particle_system::max_colours - 1 ];
		}		

		void set_colour(uint32_t i, const noob::vec4& c) noexcept(true)
		{
			if (i < max_colours)
			{
				colours[i] = c;
			}
			else
			{
				colours[max_colours - 1] = c;
			}
		}

		noob::vec4 get_colour(uint32_t i) const noexcept(true)
		{
			if (i < max_colours)
			{
				return colours[i];
			}
			return colours[max_colours - 1];
		}

		bool active;


		protected:

		uint32_t first_free, emits_per_second;

		uint64_t nanos_accum, nanos_between_emits;

		uint64_t lifespan;

		float damping, gravity_multiplier, emit_force, one_colour_in_lifetime;

		noob::vec3 center, emit_direction, emit_direction_variance, wind;

		noob::time last_update_time, current_update_time;

		rde::fixed_array<particle, max_particles> particles;

		rde::fixed_array<noob::vec4, max_colours> colours;

		noob::shape_handle shape;

		noob::reflectance_handle reflect;
	};

	// typedef noob::component<noob::particle_system> particle_systems_holder;
	typedef noob::handle<noob::particle_system> particle_system_handle;
	typedef noob::handle<noob::particle> particle_handle;
}
