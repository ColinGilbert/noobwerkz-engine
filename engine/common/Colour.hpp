#pragma once

#include <noob/math/math_funcs.hpp>


namespace noob
{
	struct colour
	{
		uint8_t r = 255;
		uint8_t g = 255;
		uint8_t b = 255;
		uint8_t a = 255;
	};

	static noob::vec4f to_floats(const noob::colour Colour) noexcept(true)
	{
		return noob::vec4f(static_cast<float>(Colour.r) / 255.0f, static_cast<float>(Colour.g) / 255.0f, static_cast<float>(Colour.b) / 255.0f, static_cast<float>(Colour.a) / 255.0f);
	}
	typedef noob::handle<vec4f> colourfp_handle;
};
