/*
* Copyright 2013 Jeremie Roy. All rights reserved.
* License: http://www.opensource.org/licenses/BSD-2-Clause
*/

#include <wchar.h> // wcslen

#include "text_metrics.h"
#include "utf8.h"

text_metrics::text_metrics(font_manager* _fontManager)
	: m_fontManager(_fontManager)
	, m_width(0)
	, m_height(0)
	, m_x(0)
	, m_line_height(0)
	, m_line_gap(0)
{
}

void text_metrics::append_text(ft_handle _fontHandle, const char* _string)
{
	const font_info& font = m_fontManager->get_font_info(_fontHandle);

	if (font.line_gap > m_line_gap)
	{
		m_line_gap = font.line_gap;
	}

	if ( (font.ascender - font.descender) > m_line_height)
	{
		m_height -= m_line_height;
		m_line_height = font.ascender - font.descender;
		m_height += m_line_height;
	}

	unicode_point codepoint = 0;
	uint32_t state = 0;

	for (; *_string; ++_string)
	{
		if (!utf8_decode(&state, (uint32_t*)&codepoint, *_string) )
		{
			const glyph_info* glyph = m_fontManager->get_glyph_info(_fontHandle, codepoint);
			if (NULL != glyph)
			{
				if (codepoint == L'\n')
				{
					m_height += m_line_gap + font.ascender - font.descender;					
					m_line_gap = font.line_gap;
					m_line_height = font.ascender - font.descender;					
					m_x = 0;
					break;
				}

				m_x += glyph->advance_x;
				if(m_x > m_width)
				{
					m_width = m_x;
				}
			}
			else
			{
				BX_CHECK(false, "Glyph not found");
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
}

void text_metrics::append_text(ft_handle _fontHandle, const wchar_t* _string)
{
	const font_info& font = m_fontManager->get_font_info(_fontHandle);

	if (font.line_gap > m_line_gap) 
	{
		m_line_gap = font.line_gap;
	}

	if ( (font.ascender - font.descender) > m_line_height)
	{
		m_height -= m_line_height;
		m_line_height = font.ascender - font.descender;
		m_height += m_line_height;
	}

	for (uint32_t ii = 0, end = (uint32_t)wcslen(_string); ii < end; ++ii)
	{
		uint32_t codepoint = _string[ii];
		const glyph_info* glyph = m_fontManager->get_glyph_info(_fontHandle, codepoint);
		if (NULL != glyph)
		{
			if (codepoint == L'\n')
			{
				m_height += m_line_gap + font.ascender - font.descender;
				m_line_gap = font.line_gap;
				m_line_height = font.ascender - font.descender;
				m_x = 0;
				break;
			}

			m_x += glyph->advance_x;
			if(m_x > m_width)
			{
				m_width = m_x;
			}
		}
		else
		{
			BX_CHECK(false, "Glyph not found");
		}
	}
}

text_line_metrics::text_line_metrics(const font_info& _fontInfo)
{
	m_line_height = _fontInfo.ascender - _fontInfo.descender + _fontInfo.line_gap;
}

uint32_t text_line_metrics::get_line_count(const char* _string) const
{
	unicode_point codepoint = 0;
	uint32_t state = 0;
	uint32_t lineCount = 1;
	for (; *_string; ++_string)
	{
		if (utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT)
		{
			if(codepoint == L'\n')
			{				
				++lineCount;
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
	return lineCount;
}

uint32_t text_line_metrics::get_line_count(const wchar_t* _string) const
{	
	uint32_t lineCount = 1;
	for ( ;*_string != L'\0'; ++_string)
	{
		if(*_string == L'\n')
		{
			++lineCount;
		}
	}
	return lineCount;
}


void text_line_metrics::get_subtext(const char* _string, uint32_t _firstLine, uint32_t _lastLine, const char*& _begin, const char*& _end)
{
	unicode_point codepoint = 0;
	uint32_t state = 0;
	// y is bottom of a text line
	uint32_t currentLine = 0;
	while(*_string && (currentLine < _firstLine))
	{
		for (; *_string; ++_string)
		{	
			if (utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT)
			{
				if (codepoint == L'\n')
				{
					++currentLine;
					++_string;
					break;
				}
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
	_begin = _string;

	while ( (*_string) && (currentLine < _lastLine) )
	{
		for (; *_string; ++_string)
		{	
			if(utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT)
			{
				if(codepoint == L'\n')
				{
					++currentLine;
					++_string;
					break;
				}
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
	_end = _string;
}

void text_line_metrics::get_subtext(const wchar_t* _string, uint32_t _firstLine, uint32_t _lastLine, const wchar_t*& _begin, const wchar_t*& _end)
{
	uint32_t currentLine = 0;	
	while ( (*_string != L'\0') && (currentLine < _firstLine) )
	{
		for ( ;*_string != L'\0'; ++_string)
		{			
			if(*_string == L'\n')
			{
				++currentLine;
				++_string;
				break;
			}
		}
	}
	_begin = _string;

	while ( (*_string != L'\0') && (currentLine < _lastLine) )
	{
		for ( ;*_string != L'\0'; ++_string)
		{			
			if(*_string == L'\n')
			{
				++currentLine;
				++_string;
				break;
			}
		}
	}
	_end = _string;	
}

void text_line_metrics::get_visible_text(const char* _string, float _top, float _bottom, const char*& _begin, const char*& _end)
{
	unicode_point codepoint = 0;
	uint32_t state = 0;
	// y is bottom of a text line
	float y = m_line_height;
	while (*_string && (y < _top) )
	{
		for (; *_string; ++_string)
		{	
			if(utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT)
			{
				if(codepoint == L'\n')
				{
					y += m_line_height;
					++_string;
					break;
				}
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
	_begin = _string;

	// y is now top of a text line
	y -= m_line_height;
	while ( (*_string) && (y < _bottom) )
	{
		for (; *_string; ++_string)
		{	
			if(utf8_decode(&state, (uint32_t*)&codepoint, *_string) == UTF8_ACCEPT)
			{
				if(codepoint == L'\n')
				{
					y += m_line_height;
					++_string;
					break;
				}
			}
		}
	}

	BX_CHECK(state == UTF8_ACCEPT, "The string is not well-formed");
	_end = _string;
}

void text_line_metrics::get_visible_text(const wchar_t* _string, float _top, float _bottom, const wchar_t*& _begin, const wchar_t*& _end)
{
	// y is bottom of a text line
	float y = m_line_height;

	const wchar_t* _textEnd = _string + wcslen(_string);

	while (y < _top)
	{
		for (const wchar_t* _current = _string; _current < _textEnd; ++_current)
		{			
			if(*_current == L'\n')
			{
				y += m_line_height;
				++_string;
				break;
			}
		}
	}
	_begin = _string;

	// y is now top of a text line
	y -= m_line_height;
	while (y < _bottom )
	{
		for (const wchar_t* _current = _string; _current < _textEnd; ++_current)
		{			
			if(*_current == L'\n')
			{
				y += m_line_height;
				++_string;
				break;
			}
		}
	}
	_end = _string;	
}
