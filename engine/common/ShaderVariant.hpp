#pragma once


#include "MathFuncs.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "Drawable3D.hpp"

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

			typedef boost::variant<noob::triplanar_renderer::uniform_info, noob::basic_renderer::uniform_info> info;
			
			void draw(const noob::model* drawable, const prepared_shaders::info& uni, const noob::mat4& model_mat = noob::identity_mat4(), uint8_t view_id = 0) const
			{
				match(uni,
				[this, drawable, model_mat, view_id] (const noob::triplanar_renderer::uniform_info& info) -> void { this->triplanar.draw(drawable, model_mat, info, view_id); },
				[this, drawable, model_mat, view_id] (const noob::basic_renderer::uniform_info& info) -> void { this->basic.draw(drawable, model_mat, info, view_id); }
				);
			}

			// TODO: Implement
			void draw_instanced(const noob::model* drawable, const prepared_shaders::info& uni, const std::vector<noob::mat4>& model_mats = { noob::identity_mat4() }, uint8_t view_id = 0) const
			{
				
			}


		protected:
			noob::basic_renderer basic;
			noob::triplanar_renderer triplanar;


	};
}
