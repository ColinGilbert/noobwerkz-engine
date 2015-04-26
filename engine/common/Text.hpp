#pragma once

#include <stdio.h>
#include <string.h>

#include <map>
#include <algorithm>

#include "MathFuncs.hpp"
#include "Logger.hpp"
#include "NoobUtils.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H

namespace noob
{
	class text
	{
		public:

			bool init(const std::string& font, int font_sz, float screen_w, float screen_h);
			void resize_screen(float w, float h);
			void draw_text(const std::string& text, float x, float y);
			void set_font_color(float r, float g, float b, float a);

		protected:
			struct glyph_handle_t
			{
				float ax; // advance.x
				float ay; // advance.y
				float bw; // bitmap.width;
				float bh; // bitmap.rows;
				float bl; // bitmap_left;
				float bt; // bitmap_top;
				float tx; // x offset of glyph in texture coordinates
				float ty;
			};

			void init();
			float red, green, blue, alpha;
			bool pack_atlas(const std::string& characters);
			/*	GLuint program;
				GLint attribute_coord;
				GLint uniform_tex;
				GLint uniform_color;
				GLuint vbo; */
			std::string filename; // Font file
			float font_size;
			float screen_width, screen_height;
			unsigned int atlas_width, atlas_height;
			std::map<char,glyph_handle_t> atlas;
			FT_Library ft;
			FT_Face face;

	};
}
