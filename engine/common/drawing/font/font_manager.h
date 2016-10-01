/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#ifndef FONT_MANAGER_H_HEADER_GUARD
#define FONT_MANAGER_H_HEADER_GUARD



#include <bx/handlealloc.h>
#include <bgfx/bgfx.h>

class atlas;

#define MAX_OPENED_FILES 64
#define MAX_OPENED_FONT  64

#define FONT_TYPE_ALPHA             UINT32_C(0x00000100) // L8
// #define FONT_TYPE_LCD               UINT32_C(0x00000200) // BGRA8
// #define FONT_TYPE_RGBA              UINT32_C(0x00000300) // BGRA8
#define FONT_TYPE_DISTANCE          UINT32_C(0x00000400) // L8
#define FONT_TYPE_DISTANCE_SUBPIXEL UINT32_C(0x00000500) // L8

struct font_info
{
	/// The font height in pixel.
	uint16_t pixelSize;
	/// Rendering type used for the font.
	int16_t fontType;

	/// The pixel extents above the baseline in pixels (typically positive).
	float ascender;
	/// The extents below the baseline in pixels (typically negative).
	float descender;
	/// The spacing in pixels between one row's descent and the next row's ascent.
	float lineGap;
	/// This field gives the maximum horizontal cursor advance for all glyphs in the font.
	float maxAdvanceWidth;
	/// The thickness of the under/hover/strike-trough line in pixels.
	float underlineThickness;
	/// The position of the underline relatively to the baseline.
	float underlinePosition;

	/// Scale to apply to glyph data.
	float scale;
};

// Glyph metrics:
// --------------
//
//                       xmin                     xmax
//                        |                         |
//                        |<-------- width -------->|
//                        |                         |
//              |         +-------------------------+----------------- ymax
//              |         |    ggggggggg   ggggg    |     ^        ^
//              |         |   g:::::::::ggg::::g    |     |        |
//              |         |  g:::::::::::::::::g    |     |        |
//              |         | g::::::ggggg::::::gg    |     |        |
//              |         | g:::::g     g:::::g     |     |        |
//    offset_x -|-------->| g:::::g     g:::::g     |  offset_y    |
//              |         | g:::::g     g:::::g     |     |        |
//              |         | g::::::g    g:::::g     |     |        |
//              |         | g:::::::ggggg:::::g     |     |        |
//              |         |  g::::::::::::::::g     |     |      height
//              |         |   gg::::::::::::::g     |     |        |
//  baseline ---*---------|---- gggggggg::::::g-----*--------      |
//            / |         |             g:::::g     |              |
//     origin   |         | gggggg      g:::::g     |              |
//              |         | g:::::gg   gg:::::g     |              |
//              |         |  g::::::ggg:::::::g     |              |
//              |         |   gg:::::::::::::g      |              |
//              |         |     ggg::::::ggg        |              |
//              |         |         gggggg          |              v
//              |         +-------------------------+----------------- ymin
//              |                                   |
//              |------------- advance_x ---------->|

/// Unicode value of a character
typedef int32_t unicode_point;

/// A structure that describe a glyph.
struct glyph_info
{
	/// Index for faster retrieval.
	int32_t glyphIndex;

	/// Glyph's width in pixels.
	float width;

	/// Glyph's height in pixels.
	float height;

	/// Glyph's left offset in pixels
	float offset_x;

	/// Glyph's top offset in pixels.
	///
	/// @remark This is the distance from the baseline to the top-most glyph
	///   scan line, upwards y coordinates being positive.
	float offset_y;

	/// For horizontal text layouts, this is the unscaled horizontal
	/// distance in pixels used to increment the pen position when the
	/// glyph is drawn as part of a string of text.
	float advance_x;

	/// For vertical text layouts, this is the unscaled vertical distance
	/// in pixels used to increment the pen position when the glyph is
	/// drawn as part of a string of text.
	float advance_y;

	/// Region index in the atlas storing textures.
	uint16_t regionIndex;
};

BGFX_HANDLE(tt_handle);
BGFX_HANDLE(ft_handle);

class font_manager
{
public:
	/// Create the font manager using an external cube atlas (doesn't take
	/// ownership of the atlas).
	font_manager(atlas* _atlas);

	/// Create the font manager and create the texture cube as BGRA8 with
	/// linear filtering.
	font_manager(uint16_t texture_side_width = 512);

	~font_manager();

	/// Retrieve the atlas used by the font manager (e.g. to add stuff to it)
	const atlas* getatlas() const
	{
		return m_atlas;
	}

	/// Load a TrueType font from a given buffer. The buffer is copied and
	/// thus can be freed or reused after this call.
	///
	/// @return invalid handle if the loading fail
	tt_handle create_ttf(const uint8_t* buffer, uint32_t _size);

	/// Unload a TrueType font (free font memory) but keep loaded glyphs.
	void destroy_ttf(tt_handle);

	/// Return a font whose height is a fixed pixel size.
	ft_handle create_font_by_pixel_size(tt_handle, uint32_t typeface_index, uint32_t pixel_size, uint32_t font_type = FONT_TYPE_ALPHA);

	/// Return a scaled child font whose height is a fixed pixel size.
	ft_handle create_scaled_font_to_pixel_size(ft_handle, uint32_t pixel_size);

	/// destroy a font (truetype or baked)
	void destroy_font(ft_handle);

	/// Preload a set of glyphs from a TrueType file.
	///
	/// @return True if every glyph could be preloaded, false otherwise if
	///   the Font is a baked font, this only do validation on the characters.
	bool preload_glyph(ft_handle, const wchar_t* _string);

	/// Preload a single glyph, return true on success.
	bool preload_glyph(ft_handle, unicode_point);

	/// Return the font descriptor of a font.
	///
	/// @remark the handle is required to be valid
	const font_info& get_font_info(ft_handle) const;

	/// Return the rendering informations about the glyph region. Load the
	/// glyph from a TrueType font if possible
	///
	const glyph_info* get_glyph_info(ft_handle, unicode_point);

	const glyph_info& get_black_glyph() const
	{
		return m_blackGlyph;
	}

private:
	struct cached_font;
	struct cached_file
	{
		uint8_t* buffer;
		uint32_t buffer_size;
	};

	void init();
	bool add_bitmap(glyph_info&, const uint8_t*);

	bool m_owns_atlas;
	atlas* m_atlas;

	bx::HandleAllocT<MAX_OPENED_FONT> m_font_handles;
	cached_font* m_cached_fonts;

	bx::HandleAllocT<MAX_OPENED_FILES> m_file_handles;
	cached_file* m_cached_files;

	glyph_info m_blackGlyph;

	//temporary buffer to raster glyph
	uint8_t* m_buffer;
};

#endif // FONT_MANAGER_H_HEADER_GUARD
