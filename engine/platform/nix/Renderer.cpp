/*
 * Renderer.cpp
 *
 *  Created on: Dec 2, 2013
 *      Author: Nishry
 */

#include "Renderer.hpp"
// #include <GLES3/gl3.h>

Renderer::Renderer()
{

}

Renderer::~Renderer()
{

}

Renderer::ContextState Renderer::OnContextCreated()
{
	Renderer::ContextState eState = Initialised;;
	eState = Renderer::Restored;
	return eState;
}
