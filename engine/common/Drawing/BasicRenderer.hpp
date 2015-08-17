#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable3D.hpp"

namespace noob
{
	class basic_renderer
	{
		public:
			struct uniform_info
			{
				uniform_info() : colour(1.0, 1.0, 1.0, 1.0) {}
				noob::vec4 colour;
			};
			
			void init();
			void draw(const noob::drawable3d& drawable3d, const noob::mat4& model_mat, const noob::basic_renderer::uniform_info& info, uint8_t view_id = 0) const;

		protected:
			noob::graphics::shader shader;
		
	};
}
