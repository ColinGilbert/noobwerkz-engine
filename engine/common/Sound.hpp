#pragma once

#include <cstdint>
#include <vector>

#include <soundio/soundio.h>

namespace noob
{
	class sound
	{
		public:
			sound() : valid(false) {}

			void init();
			void run();
			void tear_down();
			void play(const std::vector<uint16_t>& sample);

		protected:
			bool valid;
			float seconds_offset = 0.0f;
			struct SoundIo* soundio;
			struct SoundIoDevice* device;
			struct SoundIoOutStream* outstream; 
	};
}
