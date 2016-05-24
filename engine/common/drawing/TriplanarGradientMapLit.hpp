/*
#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "AnimatedModel.hpp"
#include "Light.hpp"
#include "RendererBase.hpp"

namespace noob
{
	class triplanar_gradient_map_renderer_lit  : protected renderer
	{
		public:
			
			struct uniform
			{
				// Functions used by scripting engine to access colours. TODO: Wrap around/remove
				void set_colour(unsigned int i, const noob::vec4& c)
				{
					if (i > 3) colours[3] = c;
					else colours[i] = c;
				}

				noob::vec4 get_colour(unsigned int i) const
				{
					if (i > 3) return colours[3];
					else return colours[i];
				}
				
				std::array<noob::vec4, 4> colours;
				noob::vec4 blend;
				noob::vec4 scales;
				noob::vec4 colour_positions;
				noob::graphics::texture texture_map;
			};

			void init();
			void draw(const noob::drawable*, const noob::mat4& w_mat, const noob::mat4& normal_mat, const noob::vec3& eye_pos, const noob::triplanar_gradient_map_renderer_lit::uniform&, const noob::reflectance&, const std::array<noob::light, MAX_LIGHTS>&, uint8_t view_id) const;

		protected:
	};

}
*/
