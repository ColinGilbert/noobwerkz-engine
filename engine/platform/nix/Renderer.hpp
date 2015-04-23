/*
 * Renderer.cpp
 *
 *  Created on: Dec 2, 2013
 *      Author: Nishry
 */
#pragma once

// #include <GLES3/gl3.h>

class Renderer
{
	public:
		enum ContextState
		{
			Initialised = 0,
			Invalid,
			Restored,
			RenderStateCount
		};

		Renderer();
		~Renderer();
		Renderer::ContextState OnContextCreated();
};
