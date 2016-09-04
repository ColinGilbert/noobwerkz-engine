#pragma once

#include <string>

#include "modp_numtoa.h"

#include "MathFuncs.hpp"
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

namespace noob
{
	// http://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring/217605#217605
	// trim from start
	static inline std::string& ltrim(std::string &s)
	{
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	static inline std::string& rtrim(std::string &s)
	{
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	static inline std::string& trim(std::string &s)
	{
		return ltrim(rtrim(s));
	}

	static inline std::string& trim_numeric_str(std::string& s)
	{
		std::replace(s.begin(), s.end(), '\0', ' ');
		return trim(s);
	}
		
	static std::string to_string(int32_t i)
	{
		std::string s;
		s.resize(16);
		modp_itoa10(i, &s[0]);
		return trim_numeric_str(s);		
	}

	static std::string to_string(uint32_t i)
	{
		std::string s;
		s.resize(16);
		modp_uitoa10(i, &s[0]);
		return trim_numeric_str(s);

	}

	static std::string to_string(int64_t i)
	{
		std::string s;
		s.resize(24);
		modp_litoa10(i, &s[0]);
		return trim_numeric_str(s);
	}

	static std::string to_string(uint64_t i)
	{
		std::string s;
		s.resize(24);
		modp_ulitoa10(i, &s[0]);
		return trim_numeric_str(s);
	}

	static std::string to_string(double f, uint32_t precision)
	{
		std::string s;
		s.resize(32);
		modp_dtoa2(f, &s[0], precision);
		return trim_numeric_str(s);
	}

	static std::string to_string(double f)
	{
		std::string s;
		uint32_t precision = 3;
		s.resize(32);
		modp_dtoa2(f, &s[0], precision);
		return trim_numeric_str(s);
	}

	static std::string to_string(const noob::vec3& arg)
	{
		return noob::concat("(", noob::to_string(arg.v[0]), ", ", noob::to_string(arg.v[1]), ", ", noob::to_string(arg.v[2]), ")");
	}

	static std::string to_string(const noob::vec4& arg)
	{
		return noob::concat("(", noob::to_string(arg.v[0]), ", ", noob::to_string(arg.v[1]), ", ", noob::to_string(arg.v[2]), ", ", noob::to_string(arg.v[3]), ")");
	}

	static std::string to_string(const noob::versor& arg)
	{
		return noob::concat("(", noob::to_string(arg.q[0]), ", ", noob::to_string(arg.q[1]), ", ", noob::to_string(arg.q[2]), ", ", noob::to_string(arg.q[3]), ")");
	}
}
