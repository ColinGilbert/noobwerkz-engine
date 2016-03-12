#pragma once

#include "MathFuncs.hpp"

// Note: Wanted to seal off the internals of noob::light but couldn't due to causing stupid recursive headers with the rest of the source (most notably the renderer classes.)

namespace noob
{
	struct light 
	{
		public:

		light() : rgb_falloff(noob::vec4(1.0, 1.0, 1.0, 1.0)), pos_radius(noob::vec4(0.0, 200.0, 0.0, 300.0)) {}

		void set_position(const noob::vec3& arg)
		{
			float r = pos_radius.v[3];
			pos_radius = noob::vec4(arg, r);
		}

		void set_falloff(float r)
		{
			rgb_falloff.v[3] = r;
		}

		void set_colour(const noob::vec3& arg)
		{
			float temp = rgb_falloff.v[3];
			rgb_falloff = noob::vec4(arg, temp);
		}

		void set_radius(float r)
		{
			pos_radius.v[3] = r;
		}

		noob::vec3 get_position() const
		{
			noob::vec3 pos;
			pos.v[0] = pos_radius.v[0];
			pos.v[1] = pos_radius.v[1];
			pos.v[2] = pos_radius.v[2];
			return pos;
		}

		float get_falloff() const
		{
			return rgb_falloff.v[3];
		}

		noob::vec3 get_colour() const
		{
			noob::vec3 retval;
			retval.v[0] = rgb_falloff.v[0];
			retval.v[1] = rgb_falloff.v[1];
			retval.v[2] = rgb_falloff.v[2];
			return retval;
		}
		float get_radius(float r)
		{
			return pos_radius.v[3];
		}

		noob::vec4 rgb_falloff, pos_radius;
	};
}
