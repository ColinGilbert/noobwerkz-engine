#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "AnimatedModel.hpp"
#include "Light.hpp"

namespace noob
{
	class triplanar_renderer
	{
		public:
			
			struct uniform_info
			{
				std::array<noob::vec4,4> colours;
				noob::vec3 mapping_blends;
				noob::vec3 scales;
				noob::vec2 colour_positions;
			};

			void init();
			void draw(const noob::animated_model*, const noob::mat4& world_mat, const noob::triplanar_renderer::uniform_info& info, uint8_t view_id = 0) const;

		protected:
			noob::graphics::shader shader;
	};

}
