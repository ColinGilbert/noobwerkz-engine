#pragma once

#include "bstrlib.h"
#include "bstrwrap.h"
// Too many thrd-party libs to use this last one... Yet
// #include "bsafe.h"

namespace noob
{
	class ui
	{
		public:
			void init();
			std::string get_current_state();
			void input_command(const std::string& cmd);
	};
}
