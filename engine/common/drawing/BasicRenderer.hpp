#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Model.hpp"

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
			void draw(const noob::model* model, const noob::mat4& world_matrix, const noob::basic_renderer::uniform_info& info, uint8_t view_id = 0) const;

		protected:
			noob::graphics::shader shader;
		
	};
}
