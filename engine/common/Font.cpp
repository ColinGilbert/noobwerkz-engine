#include "Font.hpp"

noob::font::~font() noexcept(true)
{
	if(atlas_valid)
	{
		ftgl::texture_atlas_delete(atlas);
	}
	if (ft_face_valid)
	{
		FT_Done_Face(ft_face);
	}
	if (ft_library_valid)
	{
		FT_Done_FreeType(ft_lib);
	}
}


bool noob::font::init_library(const std::string& Mem, const noob::vec2d Dpi) noexcept(true)
{
	dpi = Dpi;

	if (FT_Init_FreeType(&ft_lib))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Could not init FreeType library :(");
		return false;
	}

	ft_library_valid = true;

	if (FT_New_Memory_Face(ft_lib, reinterpret_cast<const FT_Byte*>(&Mem[0]), Mem.size(), 0, &ft_face))
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Failed to load font");
		ft_library_valid = false;
		return false;
	}

	ft_face_valid = true;
	return true;
	// has_vertical = FT_HAS_VERTICAL(ft_face);

}


bool noob::font::init_glyphs(const std::string& Characters, uint16_t FontSize) noexcept(true)
{
	if(ft_library_valid && ft_face_valid)
	{
		if (!is_valid_utf8(Characters))
		{
			noob::logger::log(noob::importance::ERROR, "[Fonts] Found invalid utf8 in sequence while trying to fill atlas.");
			return false;
		}

		if (atlas_valid)
		{
			ftgl::texture_atlas_delete(atlas);
			atlas_valid = false;
		}
		// Now, we can more or less assume (hope?) that our characters are be well-formed.

		uint32_t state_arg = 0;
		uint32_t codepoint_arg;
		std::vector<uint32_t> codepoints;

		for (char c = 0; c < Characters.size(); ++c)
		{
			if (!utf8_decode_hoehrmann(&state_arg, &codepoint_arg, Characters[c]))
			{
				codepoints.push_back(codepoint_arg);
			}
		}

		// FT_GlyphSlot slot = ft_face->glyph;
		if (FT_Set_Char_Size(ft_face,  FontSize * 64, FontSize * 64, static_cast<uint32_t>(dpi[0]), static_cast<uint32_t>(dpi[1])))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to set font size!"));
			return false;
		}

		font_size = FontSize;

		const uint32_t min_dim = 64;
		const uint32_t max_dim = 2048; // TODO: Set to implementation-specific max value.
		
		// Preserve squareness.
		for (uint32_t dim = min_dim; dim < max_dim + 1; dim *= 2)
		{
			if (init_glyphs_helper(codepoints, noob::vec2ui(dim, dim)))
			{
				noob::logger::log(noob::importance::INFO, noob::concat("[Font] Creating texture atlas of size ", noob::to_string(dim), "*", noob::to_string(dim), " for ", noob::to_string(codepoints.size()), " codepoints."));
				noob::graphics& gfx = noob::get_graphics();
				noob::texture_loader_2d texloader;
				texloader.from_mem_raw(noob::vec2ui(dim, dim), false, noob::pixel_format::R8, atlas->data, dim*dim);
				tex = gfx.texture_2d(texloader, false);
				return true;
			}
		}
	}
	else
	{
		noob::logger::log(noob::importance::ERROR, "[Font] Tried initializing glyphs with invalid freetype state. Didn't continue.");
	}
	return false;
}


bool noob::font::has_glyph(uint32_t CodePoint) noexcept(true)
{
	const noob::fast_hashtable::cell* test = codepoints_to_glyphs.lookup(CodePoint);
	if (codepoints_to_glyphs.is_valid(test))
	{
		return true;
	}

	return false;//noob::return_type<noob::font::glyph>();
}


bool noob::font::init_glyphs_helper(const std::vector<uint32_t>& CodePoints, const noob::vec2ui AtlasDims) noexcept(true)
{
	stored_glyphs.clear();
	codepoints_to_glyphs.clear();

	atlas = ftgl::texture_atlas_new(AtlasDims[0], AtlasDims[1], 1);

	for (uint32_t cp : CodePoints)
	{	
		const noob::fast_hashtable::cell* test = codepoints_to_glyphs.lookup(cp);
		if (!codepoints_to_glyphs.is_valid(test)) // If the codepoint isn't in there yet
		{

			// Load character glyph, skipping ones that fail.
			if (FT_Load_Char(ft_face, cp, FT_LOAD_DEFAULT))
			{
				noob::logger::log(noob::importance::ERROR, noob::concat("[Font] Failed to load glyph for codepoint ", noob::to_string(cp)));
			}
			else
			{
				FT_GlyphSlot slot = ft_face->glyph;
				FT_Render_Glyph(slot, FT_RENDER_MODE_NORMAL);

				if (slot->bitmap.width == 0 || slot->bitmap.rows == 0)
				{
					noob::logger::log(noob::importance::WARNING, noob::concat("[Font] Got glyph with area zero! Codepoint = ", noob::to_string(cp), ", width/height = ", noob::to_string(slot->bitmap.width), "/", noob::to_string(slot->bitmap.rows) ));
					break;
				}

				const ftgl::ivec4 reg = ftgl::texture_atlas_get_region(atlas, slot->bitmap.width, slot->bitmap.rows);
				
				if (reg.height > 0) // If our atlas hasn't run out of space
				{
					texture_atlas_set_region(atlas, reg.x, reg.y, reg.width, reg.height, slot->bitmap.buffer, 1);
					stored_glyphs.push_back(noob::font::glyph(noob::vec2ui(reg.width, reg.height), noob::vec2ui(reg.x, reg.y), noob::vec2ui(AtlasDims[0], AtlasDims[1])));
					noob::fast_hashtable::cell* incell = codepoints_to_glyphs.insert(cp);
					incell->value = stored_glyphs.size() - 1;
				}
				else
				{
					stored_glyphs.clear();
					codepoints_to_glyphs.clear();
					ftgl::texture_atlas_delete(atlas);
					atlas_valid = false;
					return false;
				}
			}
		}
	}
	atlas_valid = true;
	return true;
}
