/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef TEXT_BUFFER_MANAGER_H_HEADER_GUARD
#define TEXT_BUFFER_MANAGER_H_HEADER_GUARD

#include "font_manager.h"

BGFX_HANDLE(text_buf_handle);

#define MAX_TEXT_BUFFER_COUNT 64

/// type of vertex and index buffer to use with a text_buffer
struct BufferType
{
	enum Enum
	{
		Static,
		Dynamic,
		Transient,
	};
};

/// special style effect (can be combined)
enum TextStyleFlags
{
	STYLE_NORMAL = 0,
	STYLE_OVERLINE = 1,
	STYLE_UNDERLINE = 1 << 1,
	STYLE_STRIKE_THROUGH = 1 << 2,
	STYLE_BACKGROUND = 1 << 3,
};

struct text_rectangle
{
	float width, height;
};

class text_buffer;
class text_buffer_manager
{
public:

	/// text_buffer is bound to a fontManager for glyph retrieval
	/// @remark the ownership of the manager is not taken
	text_buffer_manager(font_manager* _fontManager);
	~text_buffer_manager();

	text_buf_handle create_text_buffer(uint32_t _type, BufferType::Enum _bufferType);
	void destroy_text_buffer(text_buf_handle _handle);
	void submit_text_buffer(text_buf_handle _handle, uint8_t _id, int32_t _depth = 0);

	void set_style(text_buf_handle _handle, uint32_t _flags = STYLE_NORMAL);
	void set_text_colour(text_buf_handle _handle, uint32_t _rgba = 0x000000FF);
	void set_background_colour(text_buf_handle _handle, uint32_t _rgba = 0x000000FF);

	void set_overline_colour(text_buf_handle _handle, uint32_t _rgba = 0x000000FF);
	void set_underline_colour(text_buf_handle _handle, uint32_t _rgba = 0x000000FF);
	void set_strikethrough_colour(text_buf_handle _handle, uint32_t _rgba = 0x000000FF);

	void set_pen_position(text_buf_handle _handle, float _x, float _y);

	/// Append an ASCII/utf-8 string to the buffer using current pen position and color.
	void append_text(text_buf_handle _handle, ft_handle _fontHandle, const char* _string, const char* _end = NULL);

	/// Append a wide char unicode string to the buffer using current pen position and color.
	void append_text(text_buf_handle _handle, ft_handle _fontHandle, const wchar_t* _string, const wchar_t* _end = NULL);
		
	/// Append a whole face of the atlas cube, mostly used for debugging and visualizing atlas.
	void append_atlas_face(text_buf_handle _handle, uint16_t _faceIndex);

	/// Clear the text buffer and reset its state (pen/color).
	void clear_text_buffer(text_buf_handle _handle);
	
	/// Return the rectangular size of the current text buffer (including all its content).
	text_rectangle get_rectangle(text_buf_handle _handle) const;	
	
private:
	struct buffer_cache
	{
		uint16_t indexBufferHandleIdx;
		uint16_t vertexBufferHandleIdx;
		text_buffer* textBuffer;
		BufferType::Enum bufferType;
		uint32_t fontType;
	};

	buffer_cache* m_text_buffers;
	bx::HandleAllocT<MAX_TEXT_BUFFER_COUNT> m_text_buf_handles;
	font_manager* m_fontManager;
	bgfx::VertexDecl m_vertexDecl;
	bgfx::UniformHandle u_texColor;
	bgfx::ProgramHandle m_basicProgram;
	bgfx::ProgramHandle m_distanceProgram;
	bgfx::ProgramHandle m_distanceSubpixelProgram;
};

#endif // TEXT_BUFFER_MANAGER_H_HEADER_GUARD
