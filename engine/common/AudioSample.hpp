#pragma once

#include <string>
#include <vector>

namespace noob
{
	class audio_sample
	{
		friend class mixer;
		public:
			// By default, silence.
			audio_sample() noexcept(true) : rate(44100), num_channels(1), min_before_next(0), samples(1, 0.0) {} 
			bool load_file(const std::string& filename);
			bool load_as_string(const std::string& file);

		protected:
			uint32_t rate, num_channels;
			size_t min_before_next;
			std::vector<float> samples;// = {0.0};
	};
}
