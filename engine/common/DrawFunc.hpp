#pragma once

#ifndef DRAW_FUNC_INCLUDED
#define DRAW_FUNC_INCLUDED

#include <RuntimeObjectSystem/RuntimeInclude.h>
RUNTIME_MODIFIABLE_INCLUDE;
#include <RuntimeObjectSystem/ObjectInterfacePerModule.h>
#include <RuntimeObjectSystem/IObject.h>
#include "InterfaceIds.h"

struct DrawFunc : public IObject
{
	virtual void draw(float width, float height, double delta) = 0;
};

#endif // DRAW_FUNC_INCLUDED
