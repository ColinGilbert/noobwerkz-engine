#pragma once
// std
#include <array>

// External libraries
#include <noob/math/math_funcs.hpp>

namespace noob
{
	struct shader
	{

	};

	struct terrain_shading
	{
		// The sum of the pixel's separate RGB values (basically converted into greyscale) is used to pick a colour along a greyscale gradient. These weights allow the end-user to change the nature of the final texture.
		noob::vec3f texture_weights;
		// By default, the colour gradient is set at 0.0 = colour_1, 0.25 = colour_2, 0.75 = colour_3, 1.0 colour_4 and the resulting colour is interpolated along.	
		std::array<noob::vec4f, 4> colours;
		// These offsets provide a way for the programmer to bring tweak the position of colour_2 and colour_3 and down the gradient. TODO: Remove
		noob::vec2f colour_offsets;
		// This allows the programmer to diversify the output even further!
		noob::vec3f texture_scales;
	};

	struct linked_shader
	{

	};
}
