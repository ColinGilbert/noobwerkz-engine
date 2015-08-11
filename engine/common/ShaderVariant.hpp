#pragma once


#include "MathFuncs.hpp"
#include "TriplanarGradientMap.hpp"
#include "Drawable3D.hpp"

#include <boost/variant/variant.hpp>
#include "inline_variant.hpp"

namespace noob
{
	class shaders
	{
		public:
			void init()
			{
				triplanar.init();
			}

			typedef boost::variant<noob::triplanar_renderer::uniform_info> info;
			
			void draw(const shaders::info& uni, const noob::drawable3d& drawable, const noob::mat4& model_mat = noob::identity_mat4(), uint8_t view_id = 0) const
			{
				match(uni,
				[this, drawable, model_mat, view_id] (const noob::triplanar_renderer::uniform_info& info) -> void { this->triplanar.draw(drawable, model_mat, info, view_id); });
			}

		protected:
			noob::triplanar_renderer triplanar;


	};
}
