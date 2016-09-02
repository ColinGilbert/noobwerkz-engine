#pragma once
// TODO: BOTHER! Must change the uniform types to fit! Either that or static functions! :(

#include "BasicRenderer.hpp"
#include "TriplanarGradientMap.hpp"
namespace noob
{
	union shader_union
	{
		shader_union() noexcept(true) {}
		~shader_union() noexcept(true) {}
		noob::basic_renderer::uniform basic;
		noob::triplanar_gradient_map_renderer::uniform triplanar;
	};
}
