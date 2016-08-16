#pragma once

#include <string>

#include "Globals.hpp"
#include "ComponentDefines.hpp"

#include "font_manager.h"
#include "text_buffer_manager.h"
#include "TextLabel.hpp"


namespace noob
{
	

	// TextBufferManager depends on FontManager. This class brings them together.
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
			TrueTypeHandle load_ttf(FontManager* _fm, const std::string& file_path) noexcept(true); 

			bool valid;
			float window_width, window_height;
			uint32_t font_colour;
			FontManager* font_manager;
			TextBufferManager* text_buffer_manager;
			TextBufferHandle text_to_draw;
			TrueTypeHandle ttf_handle;
			FontHandle font_handle;
	};
}
