#pragma once
#include "Graphics.hpp"
#include "MathFuncs.hpp"

namespace noob
{
	class gui_renderer
	{
		void init();
		void draw(const noob::drawable& shape, float x, float y);
	}
}
