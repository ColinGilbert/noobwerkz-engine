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
				uniform() noexcept(true) : colours({noob::vec4(1.0, 1.0, 1.0, 1.0), noob::vec4(1.0, 1.0, 1.0, 1.0), noob::vec4(0.0, 0.0, 0.0, 1.0), noob::vec4(0.0, 0.0, 0.0, 1.0) }), blend(noob::vec4(0.1, 0.2, 0.5, 0.6)), scales(1.0, 1.0, 1.0, 1.0), colour_positions(noob::vec4(0.0, 0.2, 0.7, 0.8)) {}
				std::array<noob::vec4, 4> colours;
				noob::vec4 blend;
				noob::vec4 scales;
				noob::vec4 colour_positions;
				noob::graphics::texture texture_map;
			};

			void init();
			void draw(const noob::drawable*, const noob::vec3& scales, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer::uniform&, const noob::reflectance&, const std::array<noob::light, MAX_LIGHTS>&, uint8_t view_id) const;

		protected:

			noob::renderer_base renderbase;
	};

}
