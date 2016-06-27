#pragma once

#include <cstdint>
#include <rdestl/vector.h>

namespace noob
{
	class sound_interface
	{
		public:
			sound_interface() : valid(false) {}

			void init();
			void run();
			void tear_down();
			// void play(const rde::vector<float>& sample);

		protected:
			bool valid;
	};
}
