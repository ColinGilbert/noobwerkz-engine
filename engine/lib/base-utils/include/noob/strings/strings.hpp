#pragma once

#include <algorithm>
#include <string>
#include <cstring>
#include <stdlib.h>
#include <cctype>

#include "modp_numtoa.h"


namespace noob
{

	// <cctype>'s iscntrl only works for the current locale. This is to ensure evaluation of proper ASCII.
	template <typename T>
		static bool is_ascii_control_char(T Arg)
		{
			return Arg < 32 || Arg == 127; 
		}

	static bool contains_ascii_control_chars(const std::string& Arg)
	{
		for (auto c : Arg)
		{
			if (is_ascii_control_char(c)) return true;
		}
		return false;
	}

	// TODO: Verify correctness of the trimming routines, and find which trim direction is best/fastest for our number-to-string funcs.

	// NOTE: Trimming routines from: http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
	// Trim from start
	static inline std::string& ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// Trim from end
	static inline std::string& rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// Trim from both ends
	static inline std::string& trim(std::string &s)
	{
		return ltrim(rtrim(s));
	}

	static std::string to_string(int32_t i)
	{
		std::string s(16, ' ');
		size_t num = modp_itoa10(i, &s[0]);
		return s.substr(0, num);//trim(s);		
	}

	static std::string to_string(uint32_t i)
	{
		std::string s(16, ' ');
		size_t num = modp_uitoa10(i, &s[0]);
		return s.substr(0, num);
		// return trim(s);
	}

	static std::string to_string(int64_t i)
	{
		std::string s(24, ' ');
		size_t num = modp_litoa10(i, &s[0]);
		return s.substr(0, num);
		//return trim(s);
	}

	static std::string to_string(uint64_t i)
	{
		std::string s(24, ' ');
		size_t num = modp_ulitoa10(i, &s[0]);
		return s.substr(0, num);
		//return trim(s);
	}

	static std::string to_string(double f, uint32_t precision)
	{
		std::string s(32, ' ');
		size_t num = modp_dtoa2(f, &s[0], precision);
		return s.substr(0, num);
		//return trim(s);
	}

	static std::string to_string(double f)
	{
		std::string s(32, ' ');
		uint32_t precision = 5;
		size_t num = modp_dtoa2(f, &s[0], precision);
		return s.substr(0, num);
		//return trim(s);
	}

	/////////////////////////////////////////////////////////
	// Nice, fast string concatenation function templates
	/////////////////////////////////////////////////////////

	// NOTE: Concatenation routines from:  http://stackoverflow.com/questions/18892281/most-optimized-way-of-concatenation-in-strings/18899027#18899027
	namespace concat_detail {

		template<typename>
			struct string_size_impl;

		template<size_t N>
			struct string_size_impl<const char[N]>
			{
				static constexpr size_t size(const char (&) [N]) { return N - 1; }
			};

		template<size_t N>
			struct string_size_impl<char[N]> 
			{
				static size_t size(char (&s) [N]) { return N ? strlen(s) : 0; }
			};

		template<>
			struct string_size_impl<const char*> 
			{
				static size_t size(const char* s) { return s ? strlen(s) : 0; }
			};

		template<>
			struct string_size_impl<char*>
			{
				static size_t size(char* s) { return s ? strlen(s) : 0; }
			};

		template<>
			struct string_size_impl<std::string>
			{
				static size_t size(const std::string& s) { return s.size(); }
			};

		template<typename String> size_t string_size(String&& s)
		{
			using noref_t = typename std::remove_reference<String>::type;
			using string_t = typename std::conditional<std::is_array<noref_t>::value, noref_t, typename std::remove_cv<noref_t>::type>::type;
			return string_size_impl<string_t>::size(s);
		}

		template<typename...>
			struct concatenate_impl;

		template<typename String>
			struct concatenate_impl<String>
			{
				static size_t size(String&& s) { return string_size(s); }
				static void concatenate(std::string& result, String&& s) { result += s; }
			};

		template<typename String, typename... Rest>
			struct concatenate_impl<String, Rest...>
			{
				static size_t size(String&& s, Rest&&... rest) 
				{
					return string_size(s)
						+ concatenate_impl<Rest...>::size(std::forward<Rest>(rest)...);
				}
				static void concatenate(std::string& result, String&& s, Rest&&... rest) 
				{
					result += s;
					concatenate_impl<Rest...>::concatenate(result, std::forward<Rest>(rest)...);
				}
			};

	} // namespace concat_detail

	template<typename... Strings>
		std::string concat(Strings&&... strings) 
		{
			std::string result;
			result.reserve(concat_detail::concatenate_impl<Strings...>::size(std::forward<Strings>(strings)...));
			concat_detail::concatenate_impl<Strings...>::concatenate(result, std::forward<Strings>(strings)...);
			return result;
		}

	// NOTE: utf8_check() made by Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/> -- 2005-03-30. Info below:
	// TODO: Convert to std::string
	/*
	 * The utf8_check() function scans the '\0'-terminated string starting
	 * at s. It returns a pointer to the first byte of the first malformed
	 * or overlong UTF-8 sequence found, or NULL if the string contains
	 * only correct UTF-8. It also spots UTF-8 sequences that could cause
	 * trouble if converted to UTF-16, namely surrogate characters
	 * (U+D800..U+DFFF) and non-Unicode positions (U+FFFE..U+FFFF). This
	 * routine is very likely to find a malformed sequence if the input
	 * uses any other encoding than UTF-8. It therefore can be used as a
	 * very effective heuristic for distinguishing between UTF-8 and other
	 * encodings.
	 *
	 * I wrote this code mainly as a specification of functionality; there
	 * are no doubt performance optimizations possible for certain CPUs.
	 *
	 * Markus Kuhn <http://www.cl.cam.ac.uk/~mgk25/> -- 2005-03-30
	 * License: http://www.cl.cam.ac.uk/~mgk25/short-license.html
	 */


	static unsigned char* utf8_check_kuhn(const unsigned char* Arg)
	{
		unsigned char* s = const_cast<unsigned char*>(Arg);
		while (*s) {
			if (*s < 0x80)
				/* 0xxxxxxx */
				s++;
			else if ((s[0] & 0xe0) == 0xc0) {
				/* 110XXXXx 10xxxxxx */
				if ((s[1] & 0xc0) != 0x80 ||
						(s[0] & 0xfe) == 0xc0)                        /* overlong? */
					return s;
				else
					s += 2;
			} else if ((s[0] & 0xf0) == 0xe0) {
				/* 1110XXXX 10Xxxxxx 10xxxxxx */
				if ((s[1] & 0xc0) != 0x80 ||
						(s[2] & 0xc0) != 0x80 ||
						(s[0] == 0xe0 && (s[1] & 0xe0) == 0x80) ||    /* overlong? */
						(s[0] == 0xed && (s[1] & 0xe0) == 0xa0) ||    /* surrogate? */
						(s[0] == 0xef && s[1] == 0xbf &&
						 (s[2] & 0xfe) == 0xbe))                      /* U+FFFE or U+FFFF? */
					return s;
				else
					s += 3;
			} else if ((s[0] & 0xf8) == 0xf0) {
				/* 11110XXX 10XXxxxx 10xxxxxx 10xxxxxx */
				if ((s[1] & 0xc0) != 0x80 ||
						(s[2] & 0xc0) != 0x80 ||
						(s[3] & 0xc0) != 0x80 ||
						(s[0] == 0xf0 && (s[1] & 0xf0) == 0x80) ||    /* overlong? */
						(s[0] == 0xf4 && s[1] > 0x8f) || s[0] > 0xf4) /* > U+10FFFF? */
					return s;
				else
					s += 4;
			} else
				return s;
		}

		return NULL;
	}



	// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
	// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
	//
	// Permission is hereby granted, free of charge, to any person obtaining a 
	// copy of this software and associated documentation files (the "Software"),
	// to deal in the Software without restriction, including without limitation
	// the rights to use, copy, modify, merge, publish, distribute, sublicense,
	// and/or sell copies of the Software, and to permit persons to whom the
	// Software is furnished to do so, subject to the following conditions:
	//
	// The above copyright notice and this permission notice shall be included
	// in all copies or substantial portions of the Software.
	//
	// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
	// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
	// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
	// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
	// DEALINGS IN THE SOFTWARE.


#define UTF8_ACCEPT 0
#define UTF8_REJECT 1


	static const uint8_t s_utf8d[364] =
	{
		// The first part of the table maps bytes to character classes that
		// to reduce the size of the transition table and create bitmasks.
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
		1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
		7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
		8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
		10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,

		// The second part is a transition table that maps a combination
		// of a state of the automaton and a character class to a state.
		0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
		12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
		12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
		12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
		12,36,12,12,12,12,12,12,12,12,12,12
	};

	static uint32_t utf8_decode_hoehrmann(uint32_t* _state, uint32_t* _codep, unsigned char _ch)
	{
		uint32_t byte = _ch;
		uint32_t type = s_utf8d[byte];

		*_codep = (*_state != UTF8_ACCEPT) ?
			(byte & 0x3fu) | (*_codep << 6) :
			(0xff >> type) & (byte);

		*_state = s_utf8d[256 + *_state + type];
		return *_state;
	}

	static bool is_valid_utf8(const std::string& Buffer)
	{
		if (contains_ascii_control_chars(Buffer)) return false;

		if (utf8_check_kuhn(reinterpret_cast<const uint8_t*>(Buffer.c_str())) != NULL) return false;

		uint32_t code_point;
		uint32_t state = 0;
		for (auto c : Buffer)
		{
			utf8_decode_hoehrmann(&state, &code_point, c);
			if (state != UTF8_ACCEPT) return false;
		}

		return true;
	}
}
