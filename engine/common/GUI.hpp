#pragma once


#include <memory>

#include "MathFuncs.hpp"
#include "UIFont.hpp"

namespace noob
{
	class gui
	{
		public:
			enum class font_size
			{
				BANNER, TITLE, HEADER, READING
			};
			
			void init(const std::string& prefix, size_t width, size_t height);
			void window_dims(size_t width, size_t height);
			void text(const std::string& text, float x, float y, noob::gui::font_size size = noob::gui::font_size::READING, uint32_t colour = 0xFFFF00FF);
			void frame();
			void crosshairs(bool active = false);
			
		protected:
			float window_width, window_height;

			std::unique_ptr<noob::ui_font> _banner;
			std::unique_ptr<noob::ui_font> _title;
			std::unique_ptr<noob::ui_font> _header;
			std::unique_ptr<noob::ui_font> _reading;
			
			void draw_crosshairs();
			bool crosshairs_enabled;
			noob::vec4 crosshairs_colour;
	};
}
