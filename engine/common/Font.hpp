// Simple abstraction over freetype & bgfx, refactored from bgfx's examples

// TODO: Make this use existing viewproj matrix.

#include <string>
#include <bgfx.h>

//#include <bx/fpumath.h>

#include "Drawable.hpp"
// #include "font_manager.h"
// #include "text_buffer_manager.h"

namespace noob
{
	class font
	{
		public:
			virtual ~font();
			void init(const std::string& path);
			// void change_colour(uint32_t colour);
			// void set_window_size(uint32_t window_width, uint32_t window_height);
			//void drawtext(const std::string& str, float x_pos, float y_pos);
			noob::drawable* get_drawable_text(const std::string& str);

		protected:
			//TrueTypeHandle load_ttf(FontManager* _fm, const char* _filePath);
			//FontManager* font_manager; 
			//TextBufferManager* text_buffer_manager; 
			//FontHandle ttf_fonthandle;
			//TextBufferHandle transient_text;
			//uint32_t window_width, window_height;
	};
}
