#pragma once

#include <string>
#include <vector>

namespace noob
{
	class audio_sample
	{
		friend class mixer;
		public:
			void load_file(const std::string& filename);
			void load_as_string(const std::string& file);

		protected:
			uint32_t rate, num_channels;
			std::vector<float> samples;
	};
}
