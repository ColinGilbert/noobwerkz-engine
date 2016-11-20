#pragma once

#include <cstdint>
#include <vector>
#include <limits>

#include <rdestl/algorithm.h>
#include <rdestl/basic_string.h>
#include <rdestl/fixed_array.h>
#include <rdestl/hash_map.h>

#include "ComponentDefines.hpp"
#include "AudioSample.hpp"
#include "RingBuffer.hpp"

namespace noob
{
	class mixer
	{
		friend class sound_interface;

		public:

		mixer() noexcept(true) : dirty(false), num_playing(0), max_playing(8), output_buffer_size(0) {}

		// Returns false for three reasons: First: Invalid handle (ie: sample doesn't exist.) Second: Not enough free voices. Third: Trying to play sample prior to its minimum allowed offset being reached (each sample has a minimum offset to ensurei two clips being played at the same time don't cause horrible-sounding interference.
		
		bool play_clip(noob::sample_handle, float volume) noexcept(true);
		bool loop_clip(noob::sample_handle, float volume, uint32_t max) noexcept(true);

		void tick(uint32_t num_frames) noexcept(true);


		noob::ringbuffer<double> output_buffer;


		protected:

		// void build_playlist() noexcept(true);

		struct voice_instance
		{
			// Protects against glitches (a little...)
			voice_instance() noexcept(true) : active(true), index(0) {}

			bool operator<(const noob::mixer::voice_instance& rhs) const noexcept(true)
			{
				if (index < rhs.index) return true;
				return false; 
			}

			bool operator==(const noob::mixer::voice_instance& rhs) const noexcept(true)
			{	
				if (index == rhs.index) return true;
				return false;
			}

			struct playback_info
			{
				playback_info() noexcept(true) : offset(std::numeric_limits<size_t>::max()), volume(0.0) {}

				bool operator<(const noob::mixer::voice_instance::playback_info& rhs) const noexcept(true)
				{
					if (offset < rhs.offset) return true;
					return false; 
				}

				bool operator==(const noob::mixer::voice_instance::playback_info& rhs) const noexcept(true)
				{	
					if (offset == rhs.offset) return true;
					return false;
				}

				size_t offset;
				float volume;
			};

			bool active;
			uint32_t index;
			//rde::fixed_array<noob::mixer::voice_instance::playback_info, 4> queue;
			std::array<voice_instance::playback_info, 4> queue;
		};

		bool dirty;
		uint32_t num_playing, max_playing, output_buffer_size;
		//rde::vector<voice_instance> now_playing;
		std::vector<voice_instance> now_playing;
	};
}
