#pragma once

#include "AppState.h"
#include "MathFuncs.hpp"
#include "Graphics.hpp"
#include "Drawable.hpp"
#include "UIFont.hpp"
#include "World.hpp"

namespace noob
{
	struct user_data
	{
		float window_width, window_height;
		std::unique_ptr<noob::ui_font> droid_font;
		noob::world world;
		std::unique_ptr<noob::drawable> sphere;
		bool started = false;
	};

	static std::unique_ptr<noob::user_data> data;
}
