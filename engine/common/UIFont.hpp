#pragma once

#include <string>

#include "font_manager.h"
#include "text_buffer_manager.h"

#include "UITextLabel.hpp"

namespace noob
{
	class ui_font
	{
		public:
			ui_font() noexcept(true) : valid(false) {} //font_colour(std::numeric_limits<uint32_t>::max()) {}
			void init(const std::string& filename, size_t size, float width, float height);

			void draw_text(uint8_t view_id, const std::string& message, float x, float y);
			void set_colour(uint32_t colour);
			void set_window_dims(float width, float height);

		protected:

			TrueTypeHandle load_ttf(FontManager* _fm, const std::string& file_path);

			bool valid;
			float window_width, window_height;
			uint32_t font_colour;
			FontManager* font_manager; // = new FontManager(512);
			TextBufferManager* text_buffer_manager; // = new TextBufferManager(fontManager);
			TextBufferHandle text_to_draw;
			TrueTypeHandle ttf_handle;
			FontHandle font_handle;

	};
}
