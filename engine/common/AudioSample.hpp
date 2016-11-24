#pragma once

#include <string>
#include <vector>

#include "NoobDefines.hpp"

namespace noob
{
	struct audio_sample
	{
			// By default, silence.
			audio_sample() noexcept(true) : num_channels(0), sample_rate(0), min_before_next(0), samples(1, 0) {} 
			
			bool load_file(const std::string& filename) noexcept(true);
			
			// bool load_mem(const std::string& file) noexcept(true); // TODO: IMPLEMENT

			void resample(uint32_t rate) noexcept(true);

			uint32_t num_channels, sample_rate;
			size_t min_before_next;
			std::vector<int16_t> samples;
	};
}
