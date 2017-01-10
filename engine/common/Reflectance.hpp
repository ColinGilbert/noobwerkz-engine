#pragma once

#include <noob/math/math_funcs.hpp>


namespace noob
{
	struct reflectance
	{
		reflectance() noexcept(true) : specular_shine(noob::vec4f(1.0, 1.0, 1.0, 32.0)), diffuse_albedo(noob::vec4f(0.7, 0.7, 0.7, 0.4)), emissive_fresnel(noob::vec4f(0.0, 0.0, 0.0, 0.3)) {}

		void set_specular(const noob::vec3f& arg) noexcept(true)
		{
			specular_shine.v[0] = arg.v[0];
			specular_shine.v[1] = arg.v[1];
			specular_shine.v[2] = arg.v[2];
		}

		void set_shine(float arg) noexcept(true)
		{
			specular_shine.v[3] = arg;
		}

		void set_diffuse(const noob::vec3f& arg) noexcept(true)
		{
			diffuse_albedo.v[0] = arg.v[0];
			diffuse_albedo.v[1] = arg.v[1];
			diffuse_albedo.v[2] = arg.v[2];
		}

		void set_emissive(const noob::vec3f& arg) noexcept(true)
		{
			emissive_fresnel.v[0] = arg.v[0];
			emissive_fresnel.v[1] = arg.v[1];
			emissive_fresnel.v[2] = arg.v[2];
		}

		void set_albedo(float arg) noexcept(true)
		{
			diffuse_albedo.v[3] = arg;
		}

		void set_fresnel(float arg) noexcept(true)
		{
			emissive_fresnel.v[3] = arg;
		}

		noob::vec4f specular_shine, diffuse_albedo, emissive_fresnel;
	};

	typedef noob::handle<noob::reflectance> reflectance_handle;
}
