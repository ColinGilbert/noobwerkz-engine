#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "AnimatedModel.hpp"
#include "Light.hpp"

namespace noob
{
	class triplanar_gradient_map_renderer
	{
		public:
			
			struct uniform_info
			{
				std::array<noob::vec4, 4> colours;
				noob::vec4 blend;
				noob::vec4 scales;
				noob::vec4 colour_positions;
				std::array<noob::vec4, 4> light_dir;
				noob::graphics::texture texture_map;
			};

			void init();
			void draw(const noob::drawable*, const noob::mat4& world_mat, const noob::mat4& normal_mat, const noob::triplanar_gradient_map_renderer::uniform_info& info, uint8_t view_id = 0) const;

		protected:
			noob::graphics::shader shader;
	};

}
