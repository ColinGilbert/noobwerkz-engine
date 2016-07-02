#pragma once

#include <cstdint>
#include <vector>
#include <limits>

#include <rdestl/algorithm.h>
#include <rdestl/basic_string.h>
#include <rdestl/fixed_array.h>
#include <rdestl/hash_map.h>

#include "Component.hpp"
#include "ComponentDefines.hpp"
#include "Globals.hpp"
#include "AudioSample.hpp"

namespace noob
{
	class mixer
	{
		friend class sound_interface;

		public:

		void play_clip(noob::sample_handle);

		void tick(uint32_t num_frames);

		protected:

		struct voice_instance
		{
			uint32_t index;
			size_t pos;
			float volume_left, volume_right, disparity, pitch;
		};

		rde::fixed_array<voice_instance, 4> protected_voices;
		uint32_t num_protected_voices;

		rde::fixed_array<voice_instance, 64> fx_voices;
		uint32_t num_fx_voices;
		rde::vector<uint32_t> invalid_voices;

		rde::vector<double> buffer;
		uint32_t buffer_size;
	};
}
