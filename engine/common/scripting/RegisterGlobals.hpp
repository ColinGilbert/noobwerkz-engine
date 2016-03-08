#pragma once

#include <angelscript.h>
#include "AngelVector.hpp"

#include "Globals.hpp"

namespace noob
{
	void register_globals(asIScriptEngine*, const noob::globals&);
}
