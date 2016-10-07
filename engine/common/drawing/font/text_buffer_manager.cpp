/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <stddef.h> // offsetof
#include <memory.h> // memcpy
#include <wchar.h>  // wcslen

#include "text_buffer_manager.h"
#include "utf8.h"
#include "cube_atlas.h"

#define MAX_BUFFERED_CHARACTERS (8192 - 5)

class text_buffer
{
	public:

		/// text_buffer is bound to a font_manager for glyph retrieval
		/// @remark the ownership of the manager is not taken
		text_buffer(font_manager* _font_manager);
		~text_buffer();

		void set_style(uint32_t _flags = STYLE_NORMAL)
		{
			m_styleFlags = _flags;
		}

		void set_text_colour(uint32_t _rgba = 0x000000FF)
		{
			m_textColor = toABGR(_rgba);
		}

		void set_background_colour(uint32_t _rgba = 0x000000FF)
		{
			m_backgroundColor = toABGR(_rgba);
		}

		void set_overline_colour(uint32_t _rgba = 0x000000FF)
		{
			m_overlineColor = toABGR(_rgba);
		}

		void set_underline_colour(uint32_t _rgba = 0x000000FF)
		{
			m_underlineColor = toABGR(_rgba);
		}

		void set_strikethrough_colour(uint32_t _rgba = 0x000000FF)
		{
			m_strikeThroughColor = toABGR(_rgba);
		}

		void set_pen_position(float _x, float _y)
		{
			m_penX = _x; m_penY = _y;
		}

		/// Append an ASCII/utf-8 string to the buffer using current pen
		/// position and color.
		void append_text(ft_handle _fontHandle, const char* _string, const char* _end = NULL);

		/// Append a wide char unicode string to the buffer using current pen
		/// position and color.
		void append_text(ft_handle _fontHandle, const wchar_t* _string, const wchar_t* _end = NULL);

		/// Append a whole face of the atlas cube, mostly used for debugging
		/// and visualizing atlas.
		void append_atlas_face(uint16_t _faceIndex);

		/// Clear the text buffer and reset its state (pen/color)
		void clear_text_buffer();

		/// Get pointer to the vertex buffer to submit it to the graphic card.
		const uint8_t* getVertexBuffer()
		{
			return (uint8_t*) m_vertex_buffer;
		}

		/// Number of vertex in the vertex buffer.
		uint32_t get_vertex_count() const
		{
			return m_vertex_count;
		}

		/// Size in bytes of a vertex.
		uint32_t getVertexSize() const
		{
			return sizeof(text_vertex);
		}

		/// get a pointer to the index buffer to submit it to the graphic
		const uint16_t* getIndexBuffer() const
		{
			return m_index_buffer;
		}

		/// number of index in the index buffer
		uint32_t getIndexCount() const
		{
			return m_indexCount;
		}

		/// Size in bytes of an index.
		uint32_t get_index_size() const
		{
			return sizeof(uint16_t);
		}

		uint32_t getTextColor() const
		{
			return toABGR(m_textColor);
		}

		text_rectangle get_rectangle() const
		{
			return m_rectangle;
		}

	private:
		void append_glyph(ft_handle _handle, unicode_point _codePoint);
		void vertical_center_last_line(float _txtDecalY, float _top, float _bottom);

		static uint32_t toABGR(uint32_t _rgba)
		{
			return ( ( (_rgba >>  0) & 0xff) << 24)
				| ( ( (_rgba >>  8) & 0xff) << 16)
				| ( ( (_rgba >> 16) & 0xff) <<  8)
				| ( ( (_rgba >> 24) & 0xff) <<  0)
				;
		}

		uint32_t m_styleFlags;

		// color states
		uint32_t m_textColor;

		uint32_t m_backgroundColor;
		uint32_t m_overlineColor;
		uint32_t m_underlineColor;
		uint32_t m_strikeThroughColor;

		//position states
		float m_penX;
		float m_penY;

		float m_originX;
		float m_originY;

		float m_lineAscender;
		float m_lineDescender;
		float m_line_gap;

		text_rectangle m_rectangle;
		font_manager* m_font_manager;

		void set_vertex(uint32_t _i, float _x, float _y, uint32_t _rgba, uint8_t _style = STYLE_NORMAL)
		{
			m_vertex_buffer[_i].x = _x;
			m_vertex_buffer[_i].y = _y;
			m_vertex_buffer[_i].rgba = _rgba;
			m_styleBuffer[_i] = _style;
		}

		struct text_vertex
		{
			float x, y;
			int16_t u, v, w, t;
			uint32_t rgba;
		};

		text_vertex* m_vertex_buffer;
		uint16_t* m_index_buffer;
		uint8_t* m_styleBuffer;

		uint32_t m_indexCount;
		uint32_t m_lineStartIndex;
		uint16_t m_vertex_count;
};

	text_buffer::text_buffer(font_manager* _font_manager)
	: m_styleFlags(STYLE_NORMAL)
	, m_textColor(0xffffffff)
	, m_backgroundColor(0xffffffff)
	, m_overlineColor(0xffffffff)
	, m_underlineColor(0xffffffff)
	, m_strikeThroughColor(0xffffffff)
	, m_penX(0)
	, m_penY(0)
	, m_originX(0)
	, m_originY(0)
	, m_lineAscender(0)
	, m_lineDescender(0)
	, m_line_gap(0)
	, m_font_manager(_font_manager)
	, m_vertex_buffer(new text_vertex[MAX_BUFFERED_CHARACTERS * 4])
	, m_index_buffer(new uint16_t[MAX_BUFFERED_CHARACTERS * 6])
	, m_styleBuffer(new uint8_t[MAX_BUFFERED_CHARACTERS * 4])
	, m_indexCount(0)
	, m_lineStartIndex(0)
	  , m_vertex_count(0)
{
	m_rectangle.width = 0;
	m_rectangle.height = 0;
}

text_buffer::~text_buffer()
{
	delete [] m_vertex_buffer;
	delete [] m_index_buffer;
	delete [] m_styleBuffer;
}

void text_buffer::append_text(ft_handle _fontHandle, const char* _string, const char* _end)
{
	if (m_vertex_count == 0)
	{
		m_originX = m_penX;
		m_originY = m_penY;
		m_lineDescender = 0;
		m_lineAscender = 0;
		m_line_gap = 0;
	}

	unicode_point codepoint = 0;
	uint32_t state = 0;

	if (_end == NULL)
	{
		_end = _string + strlen(_string);
	}
	// BX_CHECK(_end >= _string);

	for (; *_string && _string < _end ; ++_string)
	{
		if (utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT )
		{
			append_glyph(_fontHandle, codepoint);
		}
	}

	// BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
}

void text_buffer::append_text(ft_handle _fontHandle, const wchar_t* _string, const wchar_t* _end)
{
	if (m_vertex_count == 0)
	{
		m_originX = m_penX;
		m_originY = m_penY;
		m_lineDescender = 0;
		m_lineAscender = 0;
		m_line_gap = 0;
	}

	if (_end == NULL)
	{
		_end = _string + wcslen(_string);
	}
	// BX_CHECK(_end >= _string);

	for (const wchar_t* _current = _string; _current < _end; ++_current)
	{
		uint32_t _codePoint = *_current;
		append_glyph(_fontHandle, _codePoint);
	}
}

void text_buffer::append_atlas_face(uint16_t _faceIndex)
{
	if( m_vertex_count/4 >= MAX_BUFFERED_CHARACTERS)
	{
		return;
	}

	float x0 = m_penX;
	float y0 = m_penY;
	float x1 = x0 + (float)m_font_manager->get_atlas()->get_texture_size();
	float y1 = y0 + (float)m_font_manager->get_atlas()->get_texture_size();

	m_font_manager->get_atlas()->pack_face_layer_uv(_faceIndex
			, (uint8_t*)m_vertex_buffer
			, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u)
			, sizeof(text_vertex)
			);

	set_vertex(m_vertex_count + 0, x0, y0, m_backgroundColor);
	set_vertex(m_vertex_count + 1, x0, y1, m_backgroundColor);
	set_vertex(m_vertex_count + 2, x1, y1, m_backgroundColor);
	set_vertex(m_vertex_count + 3, x1, y0, m_backgroundColor);

	m_index_buffer[m_indexCount + 0] = m_vertex_count + 0;
	m_index_buffer[m_indexCount + 1] = m_vertex_count + 1;
	m_index_buffer[m_indexCount + 2] = m_vertex_count + 2;
	m_index_buffer[m_indexCount + 3] = m_vertex_count + 0;
	m_index_buffer[m_indexCount + 4] = m_vertex_count + 2;
	m_index_buffer[m_indexCount + 5] = m_vertex_count + 3;
	m_vertex_count += 4;
	m_indexCount += 6;
}

void text_buffer::clear_text_buffer()
{
	m_penX = 0;
	m_penY = 0;
	m_originX = 0;
	m_originY = 0;

	m_vertex_count = 0;
	m_indexCount = 0;
	m_lineStartIndex = 0;
	m_lineAscender = 0;
	m_lineDescender = 0;
	m_line_gap = 0;
	m_rectangle.width = 0;
	m_rectangle.height = 0;
}

void text_buffer::append_glyph(ft_handle _handle, unicode_point _codePoint)
{
	const glyph_info* glyph = m_font_manager->get_glyph_info(_handle, _codePoint);

	// BX_WARN(NULL != glyph, "Glyph not found (font handle %d, code point %d)", _handle.idx, _codePoint);
	
	if (NULL == glyph)
	{
		return;
	}

	const font_info& font = m_font_manager->get_font_info(_handle);

	if( m_vertex_count/4 >= MAX_BUFFERED_CHARACTERS)
	{
		return;
	}

	if (_codePoint == L'\n')
	{
		m_penX = m_originX;
		m_penY += m_line_gap + m_lineAscender -m_lineDescender;
		m_line_gap = font.line_gap;
		m_lineDescender = font.descender;
		m_lineAscender = font.ascender;
		m_lineStartIndex = m_vertex_count;
		return;
	}

	//is there a change of font size that require the text on the left to be centered again ?
	if (font.ascender > m_lineAscender
			|| (font.descender < m_lineDescender) )
	{
		if (font.descender < m_lineDescender)
		{
			m_lineDescender = font.descender;
			m_line_gap = font.line_gap;
		}

		float txtDecals = (font.ascender - m_lineAscender);
		m_lineAscender = font.ascender;
		m_line_gap = font.line_gap;
		vertical_center_last_line( (txtDecals), (m_penY - m_lineAscender), (m_penY + m_lineAscender - m_lineDescender + m_line_gap) );
	}

	float kerning = 0 * font.scale;
	m_penX += kerning;

	const glyph_info& blackGlyph = m_font_manager->get_black_glyph();
	const atlas* atlas = m_font_manager->get_atlas();

	if (m_styleFlags & STYLE_BACKGROUND
			&&  m_backgroundColor & 0xFF000000)
	{
		float x0 = (m_penX - kerning);
		float y0 = (m_penY);
		float x1 = ( (float)x0 + (glyph->advance_x) );
		float y1 = (m_penY + m_lineAscender - m_lineDescender + m_line_gap);

		atlas->pack_uv(blackGlyph.region_index, (uint8_t*)m_vertex_buffer, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u), sizeof(text_vertex));

		const uint16_t vertexCount = m_vertex_count;
		set_vertex(vertexCount + 0, x0, y0, m_backgroundColor, STYLE_BACKGROUND);
		set_vertex(vertexCount + 1, x0, y1, m_backgroundColor, STYLE_BACKGROUND);
		set_vertex(vertexCount + 2, x1, y1, m_backgroundColor, STYLE_BACKGROUND);
		set_vertex(vertexCount + 3, x1, y0, m_backgroundColor, STYLE_BACKGROUND);

		m_index_buffer[m_indexCount + 0] = vertexCount + 0;
		m_index_buffer[m_indexCount + 1] = vertexCount + 1;
		m_index_buffer[m_indexCount + 2] = vertexCount + 2;
		m_index_buffer[m_indexCount + 3] = vertexCount + 0;
		m_index_buffer[m_indexCount + 4] = vertexCount + 2;
		m_index_buffer[m_indexCount + 5] = vertexCount + 3;
		m_vertex_count += 4;
		m_indexCount += 6;
	}

	if (m_styleFlags & STYLE_UNDERLINE
			&&  m_underlineColor & 0xFF000000)
	{
		float x0 = (m_penX - kerning);
		float y0 = (m_penY + m_lineAscender - m_lineDescender * 0.5f);
		float x1 = ( (float)x0 + (glyph->advance_x) );
		float y1 = y0 + font.underline_thickness;

		atlas->pack_uv(blackGlyph.region_index, (uint8_t*)m_vertex_buffer, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u), sizeof(text_vertex));

		set_vertex(m_vertex_count + 0, x0, y0, m_underlineColor, STYLE_UNDERLINE);
		set_vertex(m_vertex_count + 1, x0, y1, m_underlineColor, STYLE_UNDERLINE);
		set_vertex(m_vertex_count + 2, x1, y1, m_underlineColor, STYLE_UNDERLINE);
		set_vertex(m_vertex_count + 3, x1, y0, m_underlineColor, STYLE_UNDERLINE);

		m_index_buffer[m_indexCount + 0] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 1] = m_vertex_count + 1;
		m_index_buffer[m_indexCount + 2] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 3] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 4] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 5] = m_vertex_count + 3;
		m_vertex_count += 4;
		m_indexCount += 6;
	}

	if (m_styleFlags & STYLE_OVERLINE && m_overlineColor & 0xFF000000)
	{
		float x0 = (m_penX - kerning);
		float y0 = (m_penY);
		float x1 = ( (float)x0 + (glyph->advance_x) );
		float y1 = y0 + font.underline_thickness;

		m_font_manager->get_atlas()->pack_uv(blackGlyph.region_index, (uint8_t*)m_vertex_buffer, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u), sizeof(text_vertex));

		set_vertex(m_vertex_count + 0, x0, y0, m_overlineColor, STYLE_OVERLINE);
		set_vertex(m_vertex_count + 1, x0, y1, m_overlineColor, STYLE_OVERLINE);
		set_vertex(m_vertex_count + 2, x1, y1, m_overlineColor, STYLE_OVERLINE);
		set_vertex(m_vertex_count + 3, x1, y0, m_overlineColor, STYLE_OVERLINE);

		m_index_buffer[m_indexCount + 0] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 1] = m_vertex_count + 1;
		m_index_buffer[m_indexCount + 2] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 3] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 4] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 5] = m_vertex_count + 3;
		m_vertex_count += 4;
		m_indexCount += 6;
	}

	if (m_styleFlags & STYLE_STRIKE_THROUGH	&&  m_strikeThroughColor & 0xFF000000)
	{
		float x0 = (m_penX - kerning);
		float y0 = (m_penY + 0.666667f * font.ascender);
		float x1 = ( (float)x0 + (glyph->advance_x) );
		float y1 = y0 + font.underline_thickness;

		atlas->pack_uv(blackGlyph.region_index, (uint8_t*)m_vertex_buffer, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u), sizeof(text_vertex));

		set_vertex(m_vertex_count + 0, x0, y0, m_strikeThroughColor, STYLE_STRIKE_THROUGH);
		set_vertex(m_vertex_count + 1, x0, y1, m_strikeThroughColor, STYLE_STRIKE_THROUGH);
		set_vertex(m_vertex_count + 2, x1, y1, m_strikeThroughColor, STYLE_STRIKE_THROUGH);
		set_vertex(m_vertex_count + 3, x1, y0, m_strikeThroughColor, STYLE_STRIKE_THROUGH);

		m_index_buffer[m_indexCount + 0] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 1] = m_vertex_count + 1;
		m_index_buffer[m_indexCount + 2] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 3] = m_vertex_count + 0;
		m_index_buffer[m_indexCount + 4] = m_vertex_count + 2;
		m_index_buffer[m_indexCount + 5] = m_vertex_count + 3;
		m_vertex_count += 4;
		m_indexCount += 6;
	}

	float x0 = m_penX + (glyph->offset_x);
	float y0 = (m_penY + m_lineAscender + (glyph->offset_y) );
	float x1 = (x0 + glyph->width);
	float y1 = (y0 + glyph->height);

	atlas->pack_uv(glyph->region_index, (uint8_t*)m_vertex_buffer, sizeof(text_vertex) * m_vertex_count + offsetof(text_vertex, u), sizeof(text_vertex));

	set_vertex(m_vertex_count + 0, x0, y0, m_textColor);
	set_vertex(m_vertex_count + 1, x0, y1, m_textColor);
	set_vertex(m_vertex_count + 2, x1, y1, m_textColor);
	set_vertex(m_vertex_count + 3, x1, y0, m_textColor);

	m_index_buffer[m_indexCount + 0] = m_vertex_count + 0;
	m_index_buffer[m_indexCount + 1] = m_vertex_count + 1;
	m_index_buffer[m_indexCount + 2] = m_vertex_count + 2;
	m_index_buffer[m_indexCount + 3] = m_vertex_count + 0;
	m_index_buffer[m_indexCount + 4] = m_vertex_count + 2;
	m_index_buffer[m_indexCount + 5] = m_vertex_count + 3;
	m_vertex_count += 4;
	m_indexCount += 6;

	m_penX += glyph->advance_x;

	if (m_penX > m_rectangle.width)
	{
		m_rectangle.width = m_penX;
	}

	if ( (m_penY +m_lineAscender - m_lineDescender+m_line_gap) > m_rectangle.height)
	{
		m_rectangle.height = (m_penY +m_lineAscender - m_lineDescender+m_line_gap);
	}
}

void text_buffer::vertical_center_last_line(float _dy, float _top, float _bottom)
{
	for (uint32_t ii = m_lineStartIndex; ii < m_vertex_count; ii += 4)
	{
		if (m_styleBuffer[ii] == STYLE_BACKGROUND)
		{
			m_vertex_buffer[ii + 0].y = _top;
			m_vertex_buffer[ii + 1].y = _bottom;
			m_vertex_buffer[ii + 2].y = _bottom;
			m_vertex_buffer[ii + 3].y = _top;
		}
		else
		{
			m_vertex_buffer[ii + 0].y += _dy;
			m_vertex_buffer[ii + 1].y += _dy;
			m_vertex_buffer[ii + 2].y += _dy;
			m_vertex_buffer[ii + 3].y += _dy;
		}
	}
}

text_buffer_manager::text_buffer_manager(font_manager* _font_manager) : m_font_manager(_font_manager)
{
	m_text_buffers = new buffer_cache[MAX_TEXT_BUFFER_COUNT];

	const bgfx::Memory* vs_font_basic;
	const bgfx::Memory* fs_font_basic;
	const bgfx::Memory* vs_font_distance_field;
	const bgfx::Memory* fs_font_distance_field;
	const bgfx::Memory* vs_font_distance_field_subpixel;
	const bgfx::Memory* fs_font_distance_field_subpixel;

	switch (bgfx::getRendererType() )
	{
		case bgfx::RendererType::Direct3D9:
			{
				vs_font_basic = bgfx::makeRef(vs_font_basic_dx9, sizeof(vs_font_basic_dx9));
				fs_font_basic = bgfx::makeRef(fs_font_basic_dx9, sizeof(fs_font_basic_dx9));
				vs_font_distance_field = bgfx::makeRef(vs_font_distance_field_dx9, sizeof(vs_font_distance_field_dx9));
				fs_font_distance_field = bgfx::makeRef(fs_font_distance_field_dx9, sizeof(fs_font_distance_field_dx9));
				vs_font_distance_field_subpixel = bgfx::makeRef(vs_font_distance_field_subpixel_dx9, sizeof(vs_font_distance_field_subpixel_dx9));
				fs_font_distance_field_subpixel = bgfx::makeRef(fs_font_distance_field_subpixel_dx9, sizeof(fs_font_distance_field_subpixel_dx9));
				break;
			}
		case bgfx::RendererType::Direct3D11:
		case bgfx::RendererType::Direct3D12:
			{
				vs_font_basic = bgfx::makeRef(vs_font_basic_dx11, sizeof(vs_font_basic_dx11));
				fs_font_basic = bgfx::makeRef(fs_font_basic_dx11, sizeof(fs_font_basic_dx11));
				vs_font_distance_field = bgfx::makeRef(vs_font_distance_field_dx11, sizeof(vs_font_distance_field_dx11));
				fs_font_distance_field = bgfx::makeRef(fs_font_distance_field_dx11, sizeof(fs_font_distance_field_dx11));
				vs_font_distance_field_subpixel = bgfx::makeRef(vs_font_distance_field_subpixel_dx11, sizeof(vs_font_distance_field_subpixel_dx11));
				fs_font_distance_field_subpixel = bgfx::makeRef(fs_font_distance_field_subpixel_dx11, sizeof(fs_font_distance_field_subpixel_dx11));
				break;
			}
		default:
			{
				vs_font_basic = bgfx::makeRef(vs_font_basic_glsl, sizeof(vs_font_basic_glsl));
				fs_font_basic = bgfx::makeRef(fs_font_basic_glsl, sizeof(fs_font_basic_glsl));
				vs_font_distance_field = bgfx::makeRef(vs_font_distance_field_glsl, sizeof(vs_font_distance_field_glsl));
				fs_font_distance_field = bgfx::makeRef(fs_font_distance_field_glsl, sizeof(fs_font_distance_field_glsl));
				vs_font_distance_field_subpixel = bgfx::makeRef(vs_font_distance_field_subpixel_glsl, sizeof(vs_font_distance_field_subpixel_glsl));
				fs_font_distance_field_subpixel = bgfx::makeRef(fs_font_distance_field_subpixel_glsl, sizeof(fs_font_distance_field_subpixel_glsl));
				break;
			}
	}

	m_basic_program = bgfx::createProgram(bgfx::createShader(vs_font_basic), bgfx::createShader(fs_font_basic), true);

	m_distance_program = bgfx::createProgram(bgfx::createShader(vs_font_distance_field), bgfx::createShader(fs_font_distance_field), true);

	m_distance_subpixel_program = bgfx::createProgram(bgfx::createShader(vs_font_distance_field_subpixel), bgfx::createShader(fs_font_distance_field_subpixel), true);

	m_vertex_decl.begin().add(bgfx::Attrib::Position,  2, bgfx::AttribType::Float).add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Int16, true).add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true).end();

	u_tex_colour = bgfx::createUniform("u_tex_colour", bgfx::UniformType::Int1);
}

text_buffer_manager::~text_buffer_manager()
{
	// BX_CHECK(m_text_buf_handles.getNumHandles() == 0, "All the text buffers must be destroyed before destroying the manager");
	delete [] m_text_buffers;

	bgfx::destroyUniform(u_tex_colour);

	bgfx::destroyProgram(m_basic_program);
	bgfx::destroyProgram(m_distance_program);
	bgfx::destroyProgram(m_distance_subpixel_program);
}

text_buf_handle text_buffer_manager::create_text_buffer(uint32_t _type, BufferType::Enum _buffer_type)
{
	uint16_t textIdx = m_text_buf_handles.alloc();
	buffer_cache& bc = m_text_buffers[textIdx];

	bc.m_text_buffer = new text_buffer(m_font_manager);
	bc.font_type = _type;
	bc.buffer_type = _buffer_type;
	bc.index_buffer_handle_idx = bgfx::invalidHandle;
	bc.vertex_buffer_handle_idx = bgfx::invalidHandle;

	text_buf_handle ret = {textIdx};
	return ret;
}

void text_buffer_manager::destroy_text_buffer(text_buf_handle _handle)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");

	buffer_cache& bc = m_text_buffers[_handle.idx];
	m_text_buf_handles.free(_handle.idx);
	delete bc.m_text_buffer;
	bc.m_text_buffer = NULL;

	if (bc.vertex_buffer_handle_idx == bgfx::invalidHandle)
	{
		return;
	}

	switch (bc.buffer_type)
	{
		case BufferType::Static:
			{
				bgfx::IndexBufferHandle ibh;
				bgfx::VertexBufferHandle vbh;
				ibh.idx = bc.index_buffer_handle_idx;
				vbh.idx = bc.vertex_buffer_handle_idx;
				bgfx::destroyIndexBuffer(ibh);
				bgfx::destroyVertexBuffer(vbh);
			}

			break;

		case BufferType::Dynamic:
			bgfx::DynamicIndexBufferHandle ibh;
			bgfx::DynamicVertexBufferHandle vbh;
			ibh.idx = bc.index_buffer_handle_idx;
			vbh.idx = bc.vertex_buffer_handle_idx;
			bgfx::destroyDynamicIndexBuffer(ibh);
			bgfx::destroyDynamicVertexBuffer(vbh);

			break;

		case BufferType::Transient: // destroyed every frame
			break;
	}
}

void text_buffer_manager::submit_text_buffer(text_buf_handle _handle, uint8_t _id, int32_t _depth)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");

	buffer_cache& bc = m_text_buffers[_handle.idx];

	const uint32_t indexSize  = bc.m_text_buffer->getIndexCount()  * bc.m_text_buffer->get_index_size();
	const uint32_t vertexSize = bc.m_text_buffer->get_vertex_count() * bc.m_text_buffer->getVertexSize();

	bgfx::ProgramHandle prog;

	if (0 == indexSize || 0 == vertexSize)
	{
		return;
	}

	bgfx::setTexture(0, u_tex_colour, m_font_manager->get_atlas()->get_texture_handle() );

	switch (bc.font_type)
	{
		case FONT_TYPE_ALPHA:
			//bgfx::setProgram(m_basic_program);
			prog = m_basic_program;
			bgfx::setState(0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
				      );
			break;

		case FONT_TYPE_DISTANCE:
			//bgfx::setProgram(m_distance_program);
			prog = m_distance_program;
			bgfx::setState(0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
				      );
			break;

		case FONT_TYPE_DISTANCE_SUBPIXEL:
			//bgfx::setProgram(m_distance_subpixel_program);
			prog = m_distance_subpixel_program;
			bgfx::setState(0
					| BGFX_STATE_RGB_WRITE
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_FACTOR, BGFX_STATE_BLEND_INV_SRC_COLOR)
					, bc.m_text_buffer->getTextColor()
				      );
			break;
	}

	switch (bc.buffer_type)
	{
		case BufferType::Static:
			{
				bgfx::IndexBufferHandle ibh;
				bgfx::VertexBufferHandle vbh;

				if (bgfx::invalidHandle == bc.vertex_buffer_handle_idx)
				{
					ibh = bgfx::createIndexBuffer(
							bgfx::copy(bc.m_text_buffer->getIndexBuffer(), indexSize)
							);

					vbh = bgfx::createVertexBuffer(
							bgfx::copy(bc.m_text_buffer->getVertexBuffer(), vertexSize)
							, m_vertex_decl
							);

					bc.vertex_buffer_handle_idx = vbh.idx;
					bc.index_buffer_handle_idx = ibh.idx;
				}
				else
				{
					vbh.idx = bc.vertex_buffer_handle_idx;
					ibh.idx = bc.index_buffer_handle_idx;
				}

				bgfx::setVertexBuffer(vbh, 0, bc.m_text_buffer->get_vertex_count() );
				bgfx::setIndexBuffer(ibh, 0, bc.m_text_buffer->getIndexCount() );
			}
			break;

		case BufferType::Dynamic:
			{
				bgfx::DynamicIndexBufferHandle ibh;
				bgfx::DynamicVertexBufferHandle vbh;

				if (bgfx::invalidHandle == bc.vertex_buffer_handle_idx )
				{
					ibh = bgfx::createDynamicIndexBuffer(bgfx::copy(bc.m_text_buffer->getIndexBuffer(), indexSize));

					vbh = bgfx::createDynamicVertexBuffer(bgfx::copy(bc.m_text_buffer->getVertexBuffer(), vertexSize), m_vertex_decl);

					bc.index_buffer_handle_idx = ibh.idx;
					bc.vertex_buffer_handle_idx = vbh.idx;
				}
				else
				{
					ibh.idx = bc.index_buffer_handle_idx;
					vbh.idx = bc.vertex_buffer_handle_idx;

					bgfx::updateDynamicIndexBuffer(ibh, 0, bgfx::copy(bc.m_text_buffer->getIndexBuffer(), indexSize));

					bgfx::updateDynamicVertexBuffer(vbh, 0, bgfx::copy(bc.m_text_buffer->getVertexBuffer(), vertexSize));
				}

				bgfx::setVertexBuffer(vbh,0, bc.m_text_buffer->get_vertex_count() );
				bgfx::setIndexBuffer(ibh, 0, bc.m_text_buffer->getIndexCount() );
			}
			break;

		case BufferType::Transient:
			{
				bgfx::TransientIndexBuffer tib;
				bgfx::TransientVertexBuffer tvb;
				bgfx::allocTransientIndexBuffer(&tib, bc.m_text_buffer->getIndexCount() );
				bgfx::allocTransientVertexBuffer(&tvb, bc.m_text_buffer->get_vertex_count(), m_vertex_decl);
				memcpy(tib.data, bc.m_text_buffer->getIndexBuffer(), indexSize);
				memcpy(tvb.data, bc.m_text_buffer->getVertexBuffer(), vertexSize);
				bgfx::setVertexBuffer(&tvb, 0, bc.m_text_buffer->get_vertex_count() );
				bgfx::setIndexBuffer(&tib, 0, bc.m_text_buffer->getIndexCount() );
			}
			break;
	}

	bgfx::submit(_id, prog, _depth);
}

void text_buffer_manager::set_style(text_buf_handle _handle, uint32_t _flags)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_style(_flags);
}

void text_buffer_manager::set_text_colour(text_buf_handle _handle, uint32_t _rgba)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_text_colour(_rgba);
}

void text_buffer_manager::set_background_colour(text_buf_handle _handle, uint32_t _rgba)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_background_colour(_rgba);
}

void text_buffer_manager::set_overline_colour(text_buf_handle _handle, uint32_t _rgba)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_overline_colour(_rgba);
}

void text_buffer_manager::set_underline_colour(text_buf_handle _handle, uint32_t _rgba)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_underline_colour(_rgba);
}

void text_buffer_manager::set_strikethrough_colour(text_buf_handle _handle, uint32_t _rgba)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_strikethrough_colour(_rgba);
}

void text_buffer_manager::set_pen_position(text_buf_handle _handle, float _x, float _y)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->set_pen_position(_x, _y);
}

void text_buffer_manager::append_text(text_buf_handle _handle, ft_handle _fontHandle, const char* _string, const char* _end)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->append_text(_fontHandle, _string, _end);
}

void text_buffer_manager::append_text(text_buf_handle _handle, ft_handle _fontHandle, const wchar_t* _string, const wchar_t* _end)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->append_text(_fontHandle, _string, _end);
}

void text_buffer_manager::append_atlas_face(text_buf_handle _handle, uint16_t _faceIndex)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->append_atlas_face(_faceIndex);
}

void text_buffer_manager::clear_text_buffer(text_buf_handle _handle)
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	bc.m_text_buffer->clear_text_buffer();
}

text_rectangle text_buffer_manager::get_rectangle(text_buf_handle _handle) const
{
	// BX_CHECK(bgfx::isValid(_handle), "Invalid handle used");
	buffer_cache& bc = m_text_buffers[_handle.idx];
	return bc.m_text_buffer->get_rectangle();
}
