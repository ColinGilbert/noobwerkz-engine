#pragma once

#ifndef UPDATE_FUNC_INCLUDED
#define UPDATE_FUNC_INCLUDED

#include <RuntimeObjectSystem/IObject.h>

struct UpdateFunc : public IObject
{
	virtual void update(double delta) = 0;
};

#endif // UPDATE_FUNC_INCLUDED
