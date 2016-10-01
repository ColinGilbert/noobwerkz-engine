/*
 * Copyright 2013 Jeremie Roy. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#ifndef TEXT_METRICS_H_HEADER_GUARD
#define TEXT_METRICS_H_HEADER_GUARD

#include "font_manager.h"

class text_metrics
{
	public:
		text_metrics(font_manager* _fontManager);

		/// Append an ASCII/utf-8 string to the metrics helper.
		void append_text(ft_handle _fontHandle, const char* _string);

		/// Append a wide char string to the metrics helper.
		void append_text(ft_handle _fontHandle, const wchar_t* _string);

		/// Return the width of the measured text.
		float get_width() const { return m_width; }

		/// Return the height of the measured text.
		float get_height() const { return m_height; }

	private:
		font_manager* m_fontManager;
		float m_width;
		float m_height;
		float m_x;
		float m_lineHeight;
		float m_lineGap;
};

/// Compute text crop area for text using a single font.
class text_line_metrics
{
	public:
		text_line_metrics(const font_info& _fontInfo);

		/// Return the height of a line of text using the given font.
		float get_line_height() const { return m_lineHeight; }

		/// Return the number of text line in the given text.
		uint32_t get_line_count(const char* _string) const;

		/// Return the number of text line in the given text.
		uint32_t get_line_count(const wchar_t* _string) const;

		/// Return the first and last character visible in the [_firstLine, _lastLine] range.
		void get_subtext(const char* _string, uint32_t _firstLine, uint32_t _lastLine, const char*& _begin, const char*& _end);

		/// Return the first and last character visible in the [_firstLine, _lastLine] range.
		void get_subtext(const wchar_t* _string, uint32_t _firstLine, uint32_t _lastLine, const wchar_t*& _begin, const wchar_t*& _end);

		/// Return the first and last character visible in the [_top, _bottom] range,
		void get_visible_text(const char* _string, float _top, float _bottom, const char*& _begin, const char*& _end);

		/// Return the first and last character visible in the [_top, _bottom] range,
		void get_visible_text(const wchar_t* _string, float _top, float _bottom, const wchar_t*& _begin, const wchar_t*& _end);

	private:
		float m_lineHeight;
};

#endif // TEXT_METRICS_H_HEADER_GUARD
