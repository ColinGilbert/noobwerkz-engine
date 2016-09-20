#pragma once

#include <algorithm>
#include <string>
#include <cstring>
#include "modp_numtoa.h"


namespace noob
{
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

}
