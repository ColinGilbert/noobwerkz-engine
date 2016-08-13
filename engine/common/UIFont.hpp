#pragma once

#include <string>

#include "font_manager.h"
#include "text_buffer_manager.h"

namespace noob
{
	struct ui_label
	{
		uint8_t view_id;
		float x, y;
		std::string message;
	};

	class ui_font
	{
		public:
			ui_font() noexcept(true) : valid(false) {} //font_colour(std::numeric_limits<uint32_t>::max()) {}
			void init(const std::string& filename, size_t size, float width, float height);

			void draw_text(uint8_t view_id, const std::string& message, float x, float y);
			void set_colour(uint32_t colour);
			void set_window_dims(float width, float height);

		protected:
			bool valid;
			float window_width, window_height;
			uint32_t font_colour;
			FontManager* fontManager; // = new FontManager(512);
			TextBufferManager* textBufferManager; // = new TextBufferManager(fontManager);
			TextBufferHandle transientText;
			TrueTypeHandle ttf_handle;
			FontHandle font_handle;

	};
}
