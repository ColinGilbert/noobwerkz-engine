#pragma once

#include "Graphics.hpp"
#include "Drawable3D.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	class hacd_renderer 
	{
		public:
			void init();
			void set_items(const std::vector<noob::mesh>& meshes);
			void set_items(const std::vector<std::vector<noob::vec3>>& hulls);
			void draw(const noob::mat4& model_mat, uint8_t view_id = 0);

			protected:
			std::vector<std::unique_ptr<noob::drawable3d>> drawable3ds;
			std::vector<noob::vec4> colours;
			noob::graphics::shader shader;
	};
}
