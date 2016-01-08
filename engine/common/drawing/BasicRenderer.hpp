#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "RendererBase.hpp"

namespace noob
{
	class basic_renderer : protected renderer
	{
		public:
			struct uniform
			{
				uniform() : colour(1.0, 1.0, 1.0, 1.0) {}
				noob::vec4 colour;
			};
			
			void init();
			void draw(const noob::drawable* model, const noob::mat4& world_matrix, const noob::mat4& normal_mat, const noob::basic_renderer::uniform&, const std::array<noob::vec4, 2>& ambient_lights, uint8_t view_id) const;

		protected:
			// noob::graphics::shader shader;
		
	};
}
