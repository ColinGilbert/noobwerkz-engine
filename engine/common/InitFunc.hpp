#pragma once

#ifndef INIT_FUNC_INCLUDED
#define INIT_FUNC_INCLUDED

#include <RuntimeObjectSystem/IObject.h>
#include "InterfaceIds.h"
#include <RuntimeObjectSystem/RuntimeInclude.h>
RUNTIME_MODIFIABLE_INCLUDE;


struct InitFunc : public IObject
{
	virtual bool init() = 0;
};

#endif // INIT_FUNC_INCLUDED
