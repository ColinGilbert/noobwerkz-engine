#pragma once

#include "Graphics.hpp"
#include "Model.hpp"
#include "MathFuncs.hpp"

#include <memory>

namespace noob
{
	class hacd_renderer 
	{
		public:
			void init();
			void set_items(const std::vector<noob::basic_mesh>& meshes);
			void draw(const noob::mat4& model_mat, uint8_t view_id = 0);

			protected:
			std::vector<std::unique_ptr<noob::model>> models;
			std::vector<noob::vec4> colours;
			noob::graphics::shader shader;
	};
}
