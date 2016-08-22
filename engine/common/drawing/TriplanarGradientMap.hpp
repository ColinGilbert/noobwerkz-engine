#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "AnimatedModel.hpp"
#include "Light.hpp"
#include "RendererBase.hpp"

namespace noob
{
	class triplanar_gradient_map_renderer
	{
		public:

			struct uniform
			{
				std::array<noob::vec4, 4> colours;
				noob::vec4 blend;
				noob::vec4 scales;
				noob::vec4 colour_positions;
				noob::graphics::texture texture_map;
			};

			void init();
			void draw(const noob::drawable*, const noob::vec3& scales, const noob::mat4& wvp_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer::uniform&, const noob::reflectance&, const std::array<noob::light, MAX_LIGHTS>&, uint8_t view_id) const;

		protected:

			noob::renderer_base renderbase;
	};

}
