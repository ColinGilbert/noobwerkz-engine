#pragma once

#ifndef UPDATE_FUNC_INCLUDED
#define UPDATE_FUNC_INCLUDED

#include <RuntimeObjectSystem/RuntimeInclude.h>
RUNTIME_MODIFIABLE_INCLUDE;

#include <RuntimeObjectSystem/IObject.h>
#include "InterfaceIds.h"

struct UpdateFunc : public IObject
{
	virtual void update(double delta) = 0;
};

#endif // UPDATE_FUNC_INCLUDED
