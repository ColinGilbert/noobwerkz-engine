#pragma once

#include <noob/strings/strings.hpp>
#include <noob/math/math_funcs.hpp>

namespace noob
{
	static std::string to_string(const noob::vec3f& arg)
	{
		return noob::concat("(", noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]), ")");
	}

	static std::string to_string(const noob::vec4f& arg)
	{
		return noob::concat("(", noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]), ", ", noob::to_string(arg[3]), ")");
	}

	static std::string to_string(const noob::versorf& arg)
	{
		return noob::concat("(", noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]), ", ", noob::to_string(arg[3]), ")");
	}
}
