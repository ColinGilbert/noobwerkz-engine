#pragma once

#include <string>

#include "Globals.hpp"
#include "ComponentDefines.hpp"

#include "font_manager.h"
#include "text_buffer_manager.h"
#include "TextLabel.hpp"


namespace noob
{


	// text_buffer_manager depends on font_manager. This class brings them together.
	class ui_font
	{
		public:
			ui_font() noexcept(true) : valid(false) {}
			void init(const std::string& filename, size_t size, float width, float height) noexcept(true);
			// void create_textbuffer() const;
			void draw_label(uint8_t view_id, noob::text_label_handle, float x, float y) noexcept(true);
			void draw_text(uint8_t view_id, const std::string& message, float x, float y) noexcept(true); 
			void set_colour(uint32_t colour) noexcept(true);
			void set_window_dims(float width, float height) noexcept(true);

		protected:
			tt_handle load_ttf(font_manager* _fm, const std::string& file_path) noexcept(true); 

			bool valid;
			float window_width, window_height;
			uint32_t font_colour;
			font_manager* font;
			text_buffer_manager* buffer;
			text_buf_handle text_to_draw;
			tt_handle ttf_handle;
			ft_handle font_handle;
	};
}
