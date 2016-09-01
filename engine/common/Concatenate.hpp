#pragma once
// http://stackoverflow.com/questions/18892281/most-optimized-way-of-concatenation-in-strings/18899027#18899027
#include <string>
#include <cstring>

namespace noob
{
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
