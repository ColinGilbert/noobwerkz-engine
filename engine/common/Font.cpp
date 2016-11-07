#include "Font.hpp"
#include "NoobUtils.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H  

FT_Library ft_lib;
FT_Face ft_face;

void noob::font::init_library(const std::string& mem) noexcept(true)
{
	if (FT_Init_FreeType(&ft_lib))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Could not init FreeType library :(");
	}

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&mem[0]), mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font");
	}
}


void noob::font::init_glyphs(const std::string& characters, uint32_t font_size) noexcept(true)
{
	FT_Set_Pixel_Sizes(ft_face, 0, font_size);

	for (char c = 0; c < characters.size(); ++c)
	{
		// Load character glyph 
		if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to load glyph for ", noob::to_string(c)));
		}
	}
}
