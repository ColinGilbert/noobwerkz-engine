#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable3D.hpp"

namespace noob
{
	class triplanar_renderer
	{
		public:
			struct uniform_info
			{
				void set_colour(size_t index, const noob::vec4& colour)
				{
					if (index < 4)
					{
						colours[index] = colour;
					}
				}

				void set_mapping_blends(const noob::vec3& blends)
				{
					mapping_blends = blends;
				}

				void set_scales(const noob::vec3& s)
				{
					scales = s;
				}

				void set_colour_positions(const noob::vec2& positions)
				{
					colour_positions = positions;
				}
				
				std::array<noob::vec4,4> colours;
				noob::vec3 mapping_blends;
				noob::vec3 scales;
				noob::vec2 colour_positions;
			};

			void init();
			void draw(const noob::drawable3d& drawable3d, const noob::mat4& model_mat, const noob::triplanar_renderer::uniform_info& info, uint8_t view_id = 0) const;

		protected:
			noob::graphics::shader shader;
	};

}
