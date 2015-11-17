#pragma once


#include "MathFuncs.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "AnimatedModel.hpp"

#include <boost/variant/variant.hpp>
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

			typedef boost::variant<noob::triplanar_gradient_map_renderer::uniform_info, noob::basic_renderer::uniform_info> info;
			
			void draw(const noob::animated_model* drawable, const prepared_shaders::info& uni, const noob::mat4& world_mat = noob::identity_mat4(), uint8_t view_id = 0) const
			{
				match(uni,
				[this, drawable, world_mat, view_id] (const noob::triplanar_gradient_map_renderer::uniform_info& info) -> void { this->triplanar.draw(drawable, world_mat, info, view_id); },
				[this, drawable, world_mat, view_id] (const noob::basic_renderer::uniform_info& info) -> void { this->basic.draw(drawable, world_mat, info, view_id); }
				);
			}

			// TODO: Implement
			void draw_instanced(const noob::animated_model* drawable, const prepared_shaders::info& uni, const std::vector<noob::mat4>& world_mats = { noob::identity_mat4() }, uint8_t view_id = 0) const
			{
				
			}


		protected:
			noob::basic_renderer basic;
			noob::triplanar_gradient_map_renderer triplanar;
	};
}
