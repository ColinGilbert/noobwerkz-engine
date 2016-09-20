#pragma once

#include "MathFuncs.hpp"


namespace noob
{
	struct reflectance
	{
		public:	
			reflectance() noexcept(true) : specular_shine(noob::vec4(1.0, 1.0, 1.0, 32.0)), diffuse(noob::vec4(0.7, 0.7, 0.7, 0.0)), emissive(noob::vec4(0.0, 0.0, 0.0, 0.0)), rough_albedo_fresnel(noob::vec4(0.3, 0.7, 0.4, 0.0)) {}
			void set_specular(const noob::vec3& arg) noexcept(true)
			{
				specular_shine.v[0] = arg.v[0];
				specular_shine.v[1] = arg.v[1];
				specular_shine.v[2] = arg.v[2];
			}

			void set_shine(float arg) noexcept(true)
			{
				specular_shine.v[3] = arg;
			}

			void set_diffuse(const noob::vec3& arg) noexcept(true)
			{
				diffuse.v[0] = arg.v[0];
				diffuse.v[1] = arg.v[1];
				diffuse.v[2] = arg.v[2];	
			}

			void set_emissive(const noob::vec3& arg) noexcept(true)
			{
				emissive.v[0] = arg.v[0];
				emissive.v[1] = arg.v[1];
				emissive.v[2] = arg.v[2];
			}

			void set_roughness(float arg) noexcept(true)
			{
				rough_albedo_fresnel.v[0] = arg;
			}

			void set_albedo(float arg) noexcept(true)
			{
				rough_albedo_fresnel.v[1] = arg;
			}

			void set_fresnel(float arg) noexcept(true)
			{
				rough_albedo_fresnel.v[2] = arg;
			}

			noob::vec4 specular_shine, diffuse, emissive, rough_albedo_fresnel;
	};
}
