/*
#pragma once

#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "RendererBase.hpp"
#include "Light.hpp"

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
			void draw(const noob::drawable* model, const noob::mat4& world_matrix, const noob::mat4& normal_mat, const noob::vec3& eye_pos,  const noob::basic_renderer::uniform&, const noob::reflectance&, const std::array<noob::light, MAX_LIGHTS>&, uint8_t view_id) const;

		protected:
		
	};
}
*/
