#pragma once

#include <boost/variant/variant.hpp>

#include "MathFuncs.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "AnimatedModel.hpp"

#include "inline_variant.hpp"

namespace noob
{
	class prepared_shaders
	{
		public:
			void init()
			{
				triplanar.init();
				basic.init();
			}

			typedef boost::variant<noob::triplanar_gradient_map_renderer::uniform, noob::basic_renderer::uniform> uniform;
			
			void draw(const noob::drawable* drawable, const prepared_shaders::uniform& uni, const noob::mat4& world_mat, const std::array<noob::vec4, 2>& ambient_lights, uint8_t view_id = 0) const
			{
				match(uni,
				[this, drawable, world_mat, ambient_lights, view_id] (const noob::triplanar_gradient_map_renderer::uniform& uni) -> void { this->triplanar.draw(drawable, world_mat, uni, ambient_lights, view_id); },
				[this, drawable, world_mat, ambient_lights, view_id] (const noob::basic_renderer::uniform& uni) -> void { this->basic.draw(drawable, world_mat, uni, ambient_lights, view_id); }
				);
			}

			// TODO: Implement
			void draw_instanced(const noob::drawable* drawable, const prepared_shaders::uniform& uni, const std::vector<noob::mat4>& world_mats = { noob::identity_mat4() }, uint8_t view_id = 0) const
			{
				
			}


		protected:
			noob::basic_renderer basic;
			noob::triplanar_gradient_map_renderer triplanar;
	};
}
