#pragma once

#include <noob/strings/strings.hpp>
#include <noob/math/math_funcs.hpp>

namespace noob
{

	template <typename T>
		static std::string to_string(const noob::vec2_type<T>& arg)
		{
			return noob::concat(noob::to_string(arg[0]), ", ", noob::to_string(arg[1]));
		}

	template <typename T>
		static std::string to_string(const noob::vec3_type<T>& arg)
		{
			return noob::concat(noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]));
		}

	template <typename T>
		static std::string to_string(const noob::vec4_type<T>& arg)
		{
			return noob::concat(noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]), ", ", noob::to_string(arg[3]));
		}

	template <typename T>
		static std::string to_string(const noob::versor_type<T>& arg)
		{
			return noob::concat(noob::to_string(arg[0]), ", ", noob::to_string(arg[1]), ", ", noob::to_string(arg[2]), ", ", noob::to_string(arg[3]));
		}
}
