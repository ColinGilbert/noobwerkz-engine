// Simple abstraction over freetype & bgfx
#pragma once

#include <string>

#include <bgfx.h>

#include "font_manager.h"
#include "text_buffer_manager.h"

namespace noob
{
	class font
	{
		public:
			virtual ~font();
			void init(const std::string& path);
			void change_colour(uint32_t colour);
			void drawtext(const std::string& str, float x_pos, float y_pos, uint32_t window_width, uint32_t window_height); 
		protected:
			TrueTypeHandle loadTtf(FontManager* _fm, const char* _filePath);
			FontManager* font_manager; 
			TextBufferManager* text_buffer_manager; 
			FontHandle ttf_fonthandle;
			TextBufferHandle transient_text;
	};
}
