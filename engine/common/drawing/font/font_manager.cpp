/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#define USE_EDTAA3 0
#include "NoobUtils.hpp"

#include <bx/macros.h>

#if BX_COMPILER_MSVC
#	define generic GenericFromFreeType // WinRT language extensions see "generic" as a keyword... this is stupid
#endif // BX_COMPILER_MSVC




BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4245) // error C4245: '=' : conversion from 'int' to 'FT_UInt', signed/unsigned mismatch
#if BX_COMPILER_MSVC || BX_COMPILER_GCC >= 40300
#pragma push_macro("interface")
#endif
#undef interface



#include <ft2build.h>
#include FT_FREETYPE_H
#include <freetype/ftglyph.h>


#if BX_COMPILER_MSVC || BX_COMPILER_GCC >= 40300
#pragma pop_macro("interface")
#endif
	BX_PRAGMA_DIAGNOSTIC_POP();


#include <bgfx/bgfx.h>
#include <math.h>

#if USE_EDTAA3
#	include <edtaa3/edtaa3func.cpp>
#else
#	define SDF_IMPLEMENTATION
#	include <sdf/sdf.h>
#endif // USE_EDTAA3

#include <wchar.h> // wcslen

#include <tinystl/allocator.h>
#include <tinystl/unordered_map.h>
	namespace stl = tinystl;

#include "font_manager.h"
#include "cube_atlas.h"

	struct FTHolder
{
	FT_Library library;
	FT_Face face;
};

class true_type_font
{
	public:
		true_type_font();
		~true_type_font();

		/// Initialize from  an external buffer
		/// @remark The ownership of the buffer is external, and you must ensure it stays valid up to this object lifetime
		/// @return true if the initialization succeed
		bool init(const uint8_t* buffer, uint32_t buffer_size, int32_t font_index, uint32_t pixel_height);

		/// return the font descriptor of the current font
		font_info get_font_info();

		/// raster a glyph as 8bit alpha to a memory buffer
		/// update the glyph_info according to the raster strategy
		/// @ remark buffer min size: glyphInfo.m_width * glyphInfo * height * sizeof(char)
		bool bakeGlyphAlpha(unicode_point code_point, glyph_info& out_glyphinfo, uint8_t* _outBuffer);

		/// raster a glyph as 32bit subpixel rgba to a memory buffer
		/// update the glyph_info according to the raster strategy
		/// @ remark buffer min size: glyphInfo.m_width * glyphInfo * height * sizeof(uint32_t)
		bool bakeGlyphSubpixel(unicode_point code_point, glyph_info& out_glyphinfo, uint8_t* _outBuffer);

		/// raster a glyph as 8bit signed distance to a memory buffer
		/// update the glyph_info according to the raster strategy
		/// @ remark buffer min size: glyphInfo.m_width * glyphInfo * height * sizeof(char)
		bool bakeGlyphDistance(unicode_point code_point, glyph_info& out_glyphinfo, uint8_t* _outBuffer);

	private:
		FTHolder* m_font;
};

true_type_font::true_type_font() : m_font(NULL)
{
}

true_type_font::~true_type_font()
{
	if (NULL != m_font)
	{
		FT_Done_Face(m_font->face);
		FT_Done_FreeType(m_font->library);
		delete m_font;
		m_font = NULL;
	}
}

bool true_type_font::init(const uint8_t* buffer, uint32_t buffer_size, int32_t _fontIndex, uint32_t _pixelHeight)
{
	BX_CHECK(m_font == NULL, "true_type_font already initialized");
	BX_CHECK( (buffer_size > 256 && buffer_size < 100000000), "TrueType buffer size is suspicious");
	BX_CHECK( (_pixelHeight > 4 && _pixelHeight < 128), "TrueType buffer size is suspicious");

	FTHolder* holder = new FTHolder;

	FT_Error error = FT_Init_FreeType(&holder->library);
	BX_WARN(!error, "FT_Init_FreeType failed.");

	if (error)
	{
		goto err0;
	}

	error = FT_New_Memory_Face(holder->library, buffer, buffer_size, _fontIndex, &holder->face);
	BX_WARN(!error, "FT_Init_FreeType failed.");

	if (error)
	{
		if (FT_Err_Unknown_File_Format == error)
		{
			goto err0;
		}

		goto err1;
	}

	error = FT_Select_Charmap(holder->face, FT_ENCODING_UNICODE);
	BX_WARN(!error, "FT_Init_FreeType failed.");

	if (error)
	{
		goto err2;
	}

	error = FT_Set_Pixel_Sizes(holder->face, 0, _pixelHeight);
	BX_WARN(!error, "FT_Init_FreeType failed.");

	if (error)
	{
		goto err2;
	}

	m_font = holder;
	return true;

err2:
	FT_Done_Face(holder->face);

err1:
	FT_Done_FreeType(holder->library);

err0:
	delete holder;
	return false;
}

font_info true_type_font::get_font_info()
{
	BX_CHECK(m_font != NULL, "true_type_font not initialized");
	BX_CHECK(FT_IS_SCALABLE(m_font->face), "Font is unscalable");

	FT_Size_Metrics metrics = m_font->face->size->metrics;

	font_info outfont_info;
	outfont_info.scale = 1.0f;
	outfont_info.ascender = metrics.ascender / 64.0f;
	outfont_info.descender = metrics.descender / 64.0f;
	outfont_info.line_gap = (metrics.height - metrics.ascender + metrics.descender) / 64.0f;
	outfont_info.max_advance_width = metrics.max_advance/ 64.0f;

	outfont_info.underlinePosition = FT_MulFix(m_font->face->underline_position, metrics.y_scale) / 64.0f;
	outfont_info.underlineThickness = FT_MulFix(m_font->face->underline_thickness, metrics.y_scale) / 64.0f;
	return outfont_info;
}

static void glyphInfoInit(glyph_info& _glyphInfo, FT_BitmapGlyph bitmap_arg, FT_GlyphSlot _slot, uint8_t* _dst, uint32_t _bpp)
{
	int32_t xx = bitmap_arg->left;
	int32_t yy = -bitmap_arg->top;
	int32_t ww = bitmap_arg->bitmap.width;
	int32_t hh = bitmap_arg->bitmap.rows;

	_glyphInfo.offset_x = (float)xx;
	_glyphInfo.offset_y = (float)yy;
	_glyphInfo.width = (float)ww;
	_glyphInfo.height = (float)hh;
	_glyphInfo.advance_x = (float)_slot->advance.x / 64.0f;
	_glyphInfo.advance_y = (float)_slot->advance.y / 64.0f;

	uint32_t dstPitch = ww * _bpp;

	uint8_t* src = bitmap_arg->bitmap.buffer;
	uint32_t srcPitch = bitmap_arg->bitmap.pitch;

	for (int32_t ii = 0; ii < hh; ++ii)
	{
		memcpy(_dst, src, dstPitch);

		_dst += dstPitch;
		src += srcPitch;
	}
}

bool true_type_font::bakeGlyphAlpha(unicode_point code_point, glyph_info& _glyphInfo, uint8_t* _outBuffer)
{
	BX_CHECK(m_font != NULL, "true_type_font not initialized");

	_glyphInfo.glyphIndex = FT_Get_Char_Index(m_font->face, code_point);

	FT_GlyphSlot slot = m_font->face->glyph;
	FT_Error error = FT_Load_Glyph(m_font->face, _glyphInfo.glyphIndex, FT_LOAD_DEFAULT);
	if (error)
	{
		return false;
	}

	FT_Glyph glyph;
	error = FT_Get_Glyph(slot, &glyph);
	if (error)
	{
		return false;
	}

	error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1);
	if (error)
	{
		return false;
	}

	FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph;

	glyphInfoInit(_glyphInfo, bitmap, slot, _outBuffer, 1);

	FT_Done_Glyph(glyph);
	return true;
}

bool true_type_font::bakeGlyphSubpixel(unicode_point code_point, glyph_info& glyph_info, uint8_t* _outBuffer)
{
	BX_CHECK(m_font != NULL, "true_type_font not initialized");

	glyph_info.glyphIndex = FT_Get_Char_Index(m_font->face, code_point);

	FT_GlyphSlot slot = m_font->face->glyph;
	FT_Error error = FT_Load_Glyph(m_font->face, glyph_info.glyphIndex, FT_LOAD_DEFAULT);
	if (error)
	{
		return false;
	}

	FT_Glyph glyph;
	error = FT_Get_Glyph(slot, &glyph);
	if (error)
	{
		return false;
	}

	error = FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_LCD, 0, 1);
	if (error)
	{
		return false;
	}

	FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph;

	glyphInfoInit(glyph_info, bitmap, slot, _outBuffer, 3);
	FT_Done_Glyph(glyph);

	return true;
}

static void makeDistanceMap(const uint8_t* in_img, uint8_t* out_img, uint32_t width, uint32_t height)
{
#if USE_EDTAA3
	int16_t* xdist = (int16_t*)malloc(width * height * sizeof(int16_t) );
	int16_t* ydist = (int16_t*)malloc(width * height * sizeof(int16_t) );
	double* gx = (double*)calloc(width * height, sizeof(double) );
	double* gy = (double*)calloc(width * height, sizeof(double) );
	double* data = (double*)calloc(width * height, sizeof(double) );
	double* outside = (double*)calloc(width * height, sizeof(double) );
	double* inside = (double*)calloc(width * height, sizeof(double) );
	uint32_t ii;

	// Convert img into double (data)
	double img_min = 255, img_max = -255;
	for (ii = 0; ii < width * height; ++ii)
	{
		double v = in_img[ii];
		data[ii] = v;
		if (v > img_max)
		{
			img_max = v;
		}

		if (v < img_min)
		{
			img_min = v;
		}
	}

	// Rescale image levels between 0 and 1
	for (ii = 0; ii < width * height; ++ii)
	{
		data[ii] = (in_img[ii] - img_min) / (img_max - img_min);
	}

	// Compute outside = edtaa3(bitmap); % Transform background (0's)
	computegradient(data, width, height, gx, gy);
	edtaa3(data, gx, gy, width, height, xdist, ydist, outside);
	for (ii = 0; ii < width * height; ++ii)
	{
		if (outside[ii] < 0)
		{
			outside[ii] = 0.0;
		}
	}

	// Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
	memset(gx, 0, sizeof(double) * width * height);
	memset(gy, 0, sizeof(double) * width * height);
	for (ii = 0; ii < width * height; ++ii)
	{
		data[ii] = 1.0 - data[ii];
	}

	computegradient(data, width, height, gx, gy);
	edtaa3(data, gx, gy, width, height, xdist, ydist, inside);
	for (ii = 0; ii < width * height; ++ii)
	{
		if (inside[ii] < 0)
		{
			inside[ii] = 0.0;
		}
	}

	// distmap = outside - inside; % Bipolar distance field
	uint8_t* out = out_img;
	for (ii = 0; ii < width * height; ++ii)
	{
		outside[ii] -= inside[ii];
		outside[ii] = 128 + outside[ii] * 16;

		if (outside[ii] < 0)
		{
			outside[ii] = 0;
		}

		if (outside[ii] > 255)
		{
			outside[ii] = 255;
		}

		out[ii] = 255 - (uint8_t) outside[ii];
	}

	free(xdist);
	free(ydist);
	free(gx);
	free(gy);
	free(data);
	free(outside);
	free(inside);
#else
	sdfBuild(out_img, width, 8.0f, in_img, width, height, width);
#endif // USE_EDTAA3
}

bool true_type_font::bakeGlyphDistance(unicode_point code_point, glyph_info& glyph_info, uint8_t* outbuffer)
{
	BX_CHECK(m_font != NULL, "true_type_font not initialized");

	glyph_info.glyphIndex = FT_Get_Char_Index(m_font->face, code_point);

	FT_Int32 loadMode = FT_LOAD_DEFAULT | FT_LOAD_NO_HINTING;
	FT_Render_Mode renderMode = FT_RENDER_MODE_NORMAL;

	FT_GlyphSlot slot = m_font->face->glyph;
	FT_Error error = FT_Load_Glyph(m_font->face, glyph_info.glyphIndex, loadMode);
	if (error)
	{
		return false;
	}

	FT_Glyph glyph;
	error = FT_Get_Glyph(slot, &glyph);
	if (error)
	{
		return false;
	}

	error = FT_Glyph_To_Bitmap(&glyph, renderMode, 0, 1);
	if (error)
	{
		return false;
	}

	FT_BitmapGlyph bitmap = (FT_BitmapGlyph)glyph;

	int32_t ww = bitmap->bitmap.width;
	int32_t hh = bitmap->bitmap.rows;

	glyphInfoInit(glyph_info, bitmap, slot, outbuffer, 1);

	FT_Done_Glyph(glyph);

	if (ww * hh > 0)
	{
		uint32_t dw = 6;
		uint32_t dh = 6;

		uint32_t nw = ww + dw * 2;
		uint32_t nh = hh + dh * 2;
		BX_CHECK(nw * nh < 128 * 128, "Buffer overflow (size %d)", nw * nh);

		uint32_t buffer_size = nw * nh * sizeof(uint8_t);

		uint8_t* alphaImg = (uint8_t*)malloc(buffer_size);
		memset(alphaImg, 0, nw * nh * sizeof(uint8_t) );

		//copy the original buffer to the temp one
		for (uint32_t ii = dh; ii < nh - dh; ++ii)
		{
			memcpy(alphaImg + ii * nw + dw, outbuffer + (ii - dh) * ww, ww);
		}

		makeDistanceMap(alphaImg, outbuffer, nw, nh);
		free(alphaImg);

		glyph_info.offset_x -= static_cast<float>(dw);
		glyph_info.offset_y -= static_cast<float>(dh);
		glyph_info.width = static_cast<float>(nw);
		glyph_info.height = static_cast<float>(nh);
	}

	return true;
}

typedef stl::unordered_map<unicode_point, glyph_info> glyph_hash_map;

// cache font data
struct font_manager::cached_font
{
	cached_font()
		: trueTypeFont(NULL)
	{
		master_handle.idx = bx::HandleAlloc::invalid;
	}

	font_info info;
	glyph_hash_map cached_glyphs;
	true_type_font* trueTypeFont;
	// an handle to a master font in case of sub distance field font
	ft_handle master_handle;
	int16_t padding;
};

#define MAX_FONT_BUFFER_SIZE (512 * 512 * 4)

font_manager::font_manager(atlas* _atlas)	: m_owns_atlas(false), m_atlas(_atlas)
{
	init();
}

font_manager::font_manager(uint16_t texture_side_width) : m_owns_atlas(true), m_atlas(new atlas(texture_side_width))
{
	init();
}

void font_manager::init()
{
	m_cached_files = new cached_file[MAX_OPENED_FILES];
	m_cached_fonts = new cached_font[MAX_OPENED_FONT];
	m_buffer = new uint8_t[MAX_FONT_BUFFER_SIZE];

	const uint32_t W = 3;
	// Create filler rectangle
	uint8_t buffer[W * W * 4];
	memset(buffer, 255, W * W * 4);

	m_blackGlyph.width = W;
	m_blackGlyph.height = W;

	///make sure the black glyph doesn't bleed by using a one pixel inner outline
	m_blackGlyph.regionIndex = m_atlas->addRegion(W, W, buffer, atlas_region::TYPE_GRAY, 1);
}

font_manager::~font_manager()
{
	BX_CHECK(m_font_handles.getNumHandles() == 0, "All the fonts must be destroyed before destroying the manager");
	delete [] m_cached_fonts;

	BX_CHECK(m_file_handles.getNumHandles() == 0, "All the font files must be destroyed before destroying the manager");
	delete [] m_cached_files;

	delete [] m_buffer;

	if (m_owns_atlas)
	{
		delete m_atlas;
	}
}

tt_handle font_manager::create_ttf(const uint8_t* buffer, uint32_t _size)
{
	uint16_t id = m_file_handles.alloc();
	BX_CHECK(id != bx::HandleAlloc::invalid, "Invalid handle used");
	m_cached_files[id].buffer = new uint8_t[_size];
	m_cached_files[id].buffer_size = _size;
	memcpy(m_cached_files[id].buffer, buffer, _size);

	tt_handle ret = { id };
	return ret;
}

void font_manager::destroy_ttf(tt_handle font_handle_ft)
{
	BX_CHECK(bgfx::isValid(font_handle_ft), "Invalid handle used");
	delete m_cached_files[font_handle_ft.idx].buffer;
	m_cached_files[font_handle_ft.idx].buffer_size = 0;
	m_cached_files[font_handle_ft.idx].buffer = NULL;
	m_file_handles.free(font_handle_ft.idx);
}

ft_handle font_manager::create_font_by_pixel_size(tt_handle ttf_handle, uint32_t typeface_index, uint32_t pixel_size, uint32_t font_type)
{
	BX_CHECK(bgfx::isValid(ttf_handle), "Invalid handle used");

	true_type_font* ttf = new true_type_font();
	if (!ttf->init(m_cached_files[ttf_handle.idx].buffer, m_cached_files[ttf_handle.idx].buffer_size, typeface_index, pixel_size) )
	{
		delete ttf;
		ft_handle invalid = { bx::HandleAlloc::invalid };
		return invalid;
	}

	const uint16_t font_idx = m_font_handles.alloc();
	BX_CHECK(font_idx != bx::HandleAlloc::invalid, "Invalid handle used");

	cached_font& font = m_cached_fonts[font_idx];
	font.trueTypeFont = ttf;
	font.info = ttf->get_font_info();
	font.info.font_type  = int16_t(font_type);
	font.info.pixelSize = uint16_t(pixel_size);
	font.cached_glyphs.clear();
	font.master_handle.idx = bx::HandleAlloc::invalid;

	ft_handle handle = { font_idx };
	return handle;
}

ft_handle font_manager::create_scaled_font_to_pixel_size(ft_handle base_font_handle, uint32_t pixel_size)
{
	BX_CHECK(bgfx::isValid(base_font_handle), "Invalid handle used");
	cached_font& base_font = m_cached_fonts[base_font_handle.idx];
	font_info& info = base_font.info;

	font_info newfont_info  = info;
	newfont_info.pixelSize = static_cast<uint16_t>(pixel_size);
	newfont_info.scale     = static_cast<float>(pixel_size) / static_cast<float>(info.pixelSize);
	newfont_info.ascender  = (newfont_info.ascender * newfont_info.scale);
	newfont_info.descender = (newfont_info.descender * newfont_info.scale);
	newfont_info.line_gap   = (newfont_info.line_gap * newfont_info.scale);
	newfont_info.max_advance_width    = (newfont_info.max_advance_width * newfont_info.scale);
	newfont_info.underlineThickness = (newfont_info.underlineThickness * newfont_info.scale);
	newfont_info.underlinePosition  = (newfont_info.underlinePosition * newfont_info.scale);

	const uint16_t font_idx = m_font_handles.alloc();
	BX_CHECK(font_idx != bx::HandleAlloc::invalid, "Invalid handle used");

	cached_font& font = m_cached_fonts[font_idx];
	font.cached_glyphs.clear();
	font.info = newfont_info;
	font.trueTypeFont = NULL;
	font.master_handle = base_font_handle;

	ft_handle handle = { font_idx };
	return handle;
}

void font_manager::destroy_font(ft_handle font_handle_ft)
{
	BX_CHECK(bgfx::isValid(font_handle_ft), "Invalid handle used");

	cached_font& font = m_cached_fonts[font_handle_ft.idx];

	if (font.trueTypeFont != NULL)
	{
		delete font.trueTypeFont;
		font.trueTypeFont = NULL;
	}

	font.cached_glyphs.clear();
	m_font_handles.free(font_handle_ft.idx);
}

bool font_manager::preload_glyph(ft_handle font_handle_ft, const wchar_t* _string)
{
	BX_CHECK(bgfx::isValid(font_handle_ft), "Invalid handle used");
	cached_font& font = m_cached_fonts[font_handle_ft.idx];

	if (NULL == font.trueTypeFont)
	{
		return false;
	}

	for (uint32_t ii = 0, end = static_cast<uint32_t>(wcslen(_string)); ii < end; ++ii)
	{
		const unicode_point code_point = _string[ii];
		if (!preload_glyph(font_handle_ft, code_point) )
		{
			return false;
		}
	}

	return true;
}

bool font_manager::preload_glyph(ft_handle font_handle_ft, unicode_point code_point)
{
	BX_CHECK(bgfx::isValid(font_handle_ft), "Invalid handle used");
	cached_font& font = m_cached_fonts[font_handle_ft.idx];
	font_info& font_info = font.info;

	glyph_hash_map::iterator iter = font.cached_glyphs.find(code_point);
	if (iter != font.cached_glyphs.end() )
	{
		return true;
	}

	if (NULL != font.trueTypeFont)
	{
		glyph_info glyphInfo;

		switch (font.info.font_type)
		{
			case FONT_TYPE_ALPHA:
				font.trueTypeFont->bakeGlyphAlpha(code_point, glyphInfo, m_buffer);
				break;

			case FONT_TYPE_DISTANCE:
				font.trueTypeFont->bakeGlyphDistance(code_point, glyphInfo, m_buffer);
				break;

			case FONT_TYPE_DISTANCE_SUBPIXEL:
				font.trueTypeFont->bakeGlyphDistance(code_point, glyphInfo, m_buffer);
				break;

			default:
				BX_CHECK(false, "TextureType not supported yet");
		}

		if (!add_bitmap(glyphInfo, m_buffer) )
		{
			return false;
		}

		glyphInfo.advance_x = (glyphInfo.advance_x * font_info.scale);
		glyphInfo.advance_y = (glyphInfo.advance_y * font_info.scale);
		glyphInfo.offset_x = (glyphInfo.offset_x * font_info.scale);
		glyphInfo.offset_y = (glyphInfo.offset_y * font_info.scale);
		glyphInfo.height = (glyphInfo.height * font_info.scale);
		glyphInfo.width = (glyphInfo.width * font_info.scale);

		font.cached_glyphs[code_point] = glyphInfo;
		return true;
	}

	if (isValid(font.master_handle) && preload_glyph(font.master_handle, code_point))
	{
		const glyph_info* glyph = get_glyph_info(font.master_handle, code_point);

		glyph_info glyphInfo = *glyph;
		glyphInfo.advance_x = (glyphInfo.advance_x * font_info.scale);
		glyphInfo.advance_y = (glyphInfo.advance_y * font_info.scale);
		glyphInfo.offset_x = (glyphInfo.offset_x * font_info.scale);
		glyphInfo.offset_y = (glyphInfo.offset_y * font_info.scale);
		glyphInfo.height = (glyphInfo.height * font_info.scale);
		glyphInfo.width = (glyphInfo.width * font_info.scale);

		font.cached_glyphs[code_point] = glyphInfo;
		return true;
	}

	return false;
}

const font_info& font_manager::get_font_info(ft_handle font_handle_ft) const
{
	BX_CHECK(bgfx::isValid(font_handle_ft), "Invalid handle used");
	return m_cached_fonts[font_handle_ft.idx].info;
}

const glyph_info* font_manager::get_glyph_info(ft_handle font_handle_ft, unicode_point code_point)
{
	const glyph_hash_map& cached_glyphs = m_cached_fonts[font_handle_ft.idx].cached_glyphs;

	glyph_hash_map::const_iterator it = cached_glyphs.find(code_point);

	if (it == cached_glyphs.end())
	{

		if (!preload_glyph(font_handle_ft, code_point))
		{

			return NULL;
		}

		it = cached_glyphs.find(code_point);
	}

	BX_CHECK(it != cached_glyphs.end(), "Failed to preload glyph.");
	return &it->second;
}

bool font_manager::add_bitmap(glyph_info& glyph_info, const uint8_t* data_out)
{
	glyph_info.regionIndex = m_atlas->addRegion(static_cast<uint16_t>(ceil(glyph_info.width)), static_cast<uint16_t>(ceil(glyph_info.height)), data_out, atlas_region::TYPE_GRAY);
	return true;
}
