#pragma once

#include "MathFuncs.hpp"

// Note: Wanted to seal off the internals of noob::light but couldn't due to causing stupid recursive headers with the rest of the source (most notably the renderer classes.)

namespace noob
{
	struct light 
	{
		public:

		light() : pos_radius(noob::vec4(0.0, 200.0, 0.0, 100.0)), rgb_inner_r(noob::vec4(1.0, 1.0, 1.0, 20.0)) {}

		void set_position(const noob::vec3& arg)
		{
			float r = pos_radius.v[3];
			pos_radius = noob::vec4(arg, r);
		}

		void set_spot_radius(float r)
		{
			rgb_inner_r.v[3] = r;
		}

		void set_colour(const noob::vec3& arg)
		{
			rgb_inner_r = noob::vec4(arg, rgb_inner_r.v[3]);
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

		float get_spot_radius() const
		{
			return rgb_inner_r.v[3];
		}

		noob::vec3 get_colour() const
		{
			noob::vec3 retval;
			retval.v[0] = rgb_inner_r.v[0];
			retval.v[1] = rgb_inner_r.v[1];
			retval.v[2] = rgb_inner_r.v[2];
			return retval;
		}
		float get_radius(float r)
		{
			return pos_radius.v[3];
		}


		noob::vec4 rgb_inner_r, pos_radius;
	};

	struct reflectance
	{ 
		reflectance() : specular_shine(noob::vec4(1.0, 1.0, 1.0, 32.0)), diffuse(noob::vec4(1.0, 1.0, 1.0, 1.0)), ambient(noob::vec4(0.6, 0.6, 0.6, 0.6)), emissive(noob::vec4(0.0, 0.0, 0.0, 0.0)) {}
		
		void set_specular(const noob::vec3& arg)
		{
			specular_shine.v[0] = arg.v[0];
			specular_shine.v[1] = arg.v[1];
			specular_shine.v[2] = arg.v[2];
		}

		void set_specular_shiny(float arg)
		{
			specular_shine.v[3] = arg;
		}

		void set_diffuse(const noob::vec3& arg)
		{
			diffuse.v[0] = arg.v[0];
			diffuse.v[1] = arg.v[1];
			diffuse.v[2] = arg.v[2];	
		}
		
		void set_ambient(const noob::vec3& arg)
		{
			ambient.v[0] = arg.v[0];
			ambient.v[1] = arg.v[1];
			ambient.v[2] = arg.v[2];
		}

		void set_emissive(const noob::vec3& arg)
		{
			emissive.v[0] = arg.v[0];
			emissive.v[1] = arg.v[1];
			emissive.v[2] = arg.v[2];
		}

		noob::vec4 specular_shine, diffuse, ambient, emissive;

	};
}
