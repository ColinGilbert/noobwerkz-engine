#pragma once

#include <string>

#include "modp_numtoa.h"

#include "MathFuncs.hpp"


namespace noob
{
	static std::string to_string(int32_t i)
	{
		std::string s;
		s.reserve(16);
		modp_itoa10(i, &s[0]);
		return s;
	}

	static std::string to_string(uint32_t i)
	{
		std::string s;
		s.reserve(16);
		modp_uitoa10(i, &s[0]);
		return s;
	}

	static std::string to_string(int64_t i)
	{
		std::string s;
		s.reserve(24);
		modp_litoa10(i, &s[0]);
		return s;
	}

	static std::string to_string(uint64_t i)
	{
		std::string s;
		s.reserve(24);
		modp_ulitoa10(i, &s[0]);
		return s;
	}

	static std::string to_string(double f, uint32_t precision)
	{
		std::string s;
		s.reserve(32);
		modp_dtoa2(f, &s[0], precision);
		return s;
	}
	static std::string to_string(double f)
	{
		std::string s;
		uint32_t precision = 24;
		s.reserve(32);
		modp_dtoa2(f, &s[0], precision);
		return s;
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
