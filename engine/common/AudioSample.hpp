#pragma once

#include <string>
#include <vector>

namespace noob
{
	struct audio_sample
	{
			// By default, silence.
			audio_sample() noexcept(true) : num_channels(1), rate(44100), min_before_next(1), samples(1, 0.0) {} 
			
			bool load_file(const std::string& filename) noexcept(true);
			bool load_mem(const std::string& file) noexcept(true);

			uint32_t num_channels;
			size_t rate, min_before_next;
			std::vector<float> samples;
	};
}
