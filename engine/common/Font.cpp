#include "Font.hpp"

// #include <SkylineBinPack.h>


void noob::font::init_library(const std::string& Mem, const noob::vec2d Dpi) noexcept(true)
{
	dpi = Dpi;

	if (FT_Init_FreeType(&ft_lib))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Could not init FreeType library :(");
		return;
	}

	ft_library_valid = true;

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&Mem[0]), Mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font");
		ft_library_valid = false;
		return;
	}

	ft_face_valid = true;

	has_vertical = FT_HAS_VERTICAL(ft_face);

}


bool noob::font::init_glyphs(const std::string& Characters, uint16_t FontSize) noexcept(true)
{
	font_size = FontSize;
	if(ft_library_valid && ft_face_valid)
	{
		// rbp::SkylineBinPack packer;	

		// FT_GlyphSlot slot = ft_face->glyph;
		if (FT_Set_Char_Size(ft_face, 0, FontSize * 64, static_cast<uint32_t>(dpi[0]), static_cast<uint32_t>(dpi[1])))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to set font size!"));

			return false;
		}
		const uint32_t min_dim = 64;
		const uint32_t max_dim = 2048; // TODO: Set to implementation-specific max value.
		
		for (uint32_t dim = min_dim; dim < max_dim; dim *= 2)
		{
			atlas = ftgl::texture_atlas_new(dim, dim, 1);
			const bool valid = init_glyphs_helper(Characters, noob::vec2ui(dim, dim));
			if (valid)
			{
				// TODO: Texture buffer upload code here:

				return true;
			}
			else
			{
				ftgl::texture_atlas_delete(atlas);
			}
		}
	}

	return false;
}

/*
   noob::return_type<noob::font::glyph> noob::font::get_glyph(char arg) noexcept(true)
   {
   return noob::return_type<noob::font::glyph>(false, results);
   }
   */

bool noob::font::init_glyphs_helper(const std::string& Characters, const noob::vec2ui AtlasDims) noexcept(true)
{
	stored_glyphs.clear();
	
	for (char c = 0; c < Characters.size(); ++c)
	{
		// Load character glyph, skipping ones that fail.
		if (FT_Load_Char(ft_face, c, FT_LOAD_RENDER))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to load glyph for ", noob::to_string(c)));
		}
		else
		{
			FT_GlyphSlot slot = ft_face->glyph;
			const ftgl::ivec4 reg = texture_atlas_get_region(atlas, slot->bitmap.width, slot->bitmap.rows);
			if (reg.height > 0)
			{
				texture_atlas_set_region(atlas, reg.x, reg.y, reg.width, reg.height, slot->bitmap.buffer, 1);
				stored_glyphs.push_back(noob::font::glyph(slot->metrics, noob::vec2ui(reg.width, reg.height), noob::vec2ui(reg.x, reg.y), noob::vec2ui(AtlasDims[0], AtlasDims[1])));
				// TODO: Add hashtable insert code for character lookups.
				
			}
			else
			{
				return false;
			}
		}
	}

	return true;
}
