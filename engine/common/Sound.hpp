#pragma once

#include <cstdint>
#include <rdestl/vector.h>

namespace noob
{
	class sound
	{
		public:
			sound() : valid(false) {}

			void init();
			void run();
			void tear_down();
			void play(const rde::vector<uint16_t>& sample);

		protected:
			bool valid;
			float seconds_offset = 0.0f;
			// struct SoundIo* soundio;
			// struct SoundIoDevice* device;
			// struct SoundIoOutStream* outstream; 
	};
}
