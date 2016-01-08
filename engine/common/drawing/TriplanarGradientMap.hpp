#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "AnimatedModel.hpp"
#include "Light.hpp"
#include "RendererBase.hpp"

namespace noob
{
	class triplanar_gradient_map_renderer : protected renderer
	{
		public:
			
			struct uniform
			{
				std::array<noob::vec4, 4> colours;
				noob::vec4 blend;
				noob::vec4 scales;
				noob::vec4 colour_positions;
				// std::array<noob::vec4, 4> light_dir;
				noob::graphics::texture texture_map;
			};

			void init();
			void draw(const noob::drawable*, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::triplanar_gradient_map_renderer::uniform&, const std::array<vec4, 2>& ambient_lights, uint8_t view_id = 0) const;

		protected:
			// noob::graphics::shader shader;
			// bool program_valid;
	};

}
