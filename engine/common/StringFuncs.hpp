#pragma once

#include <noob/strings/strings.hpp>
#include "MathFuncs.hpp"

namespace noob
{
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
