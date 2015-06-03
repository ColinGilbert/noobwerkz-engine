#pragma once

#ifndef DRAW_FUNC_INCLUDED
#define DRAW_FUNC_INCLUDED

#include <RuntimeObjectSystem/IObject.h>

struct DrawFunc : public IObject
{
	virtual void draw(float width, float height, double delta) = 0;
};

#endif // DRAW_FUNC_INCLUDED
