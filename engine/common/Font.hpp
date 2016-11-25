#pragma once

#include <string>
#include <vector>

namespace noob
{
	class font
	{
		public:
			struct glyph_info
			{
				float advance_x;
				float advance_y;

				float bitmap_width;
				float bitmap_height;

				float bitmap_left;
				float bitmap_top;

				float texture_x_offset;
				float texture_y_offset;
			};

			void init_library(const std::string& mem) noexcept(true);

			void init_glyphs(const std::string& characters, uint32_t font_size) noexcept(true);

			noob::font::glyph_info get_glyph(char) noexcept(true);

		protected:
			std::vector<noob::font::glyph_info> glyphs;
	};
}
