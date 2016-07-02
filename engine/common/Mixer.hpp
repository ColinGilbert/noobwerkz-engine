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

		// Returns false for three reasons: First: Invalid handle (ie: sample doesn't exist.) Second: Not enough free voices. Third: Trying to play sample prior to its minimum allowed offset being reached (each sample has a minimum offset to ensurei two clips being played at the same time don't cause horrible-sounding interference.
		bool play_clip(noob::sample_handle, float volume);
		bool loop_clip(noob::sample_handle, float volume, uint32_t max);
		void tick(uint32_t num_frames);

		protected:

		void build_playlist();

		struct voice_instance
		{

			bool operator<(const noob::mixer::voice_instance& rhs)
			{
				if (index < rhs.index) return true;
				if (index == rhs.index && volume < rhs.volume) return true;
				return false; 
			}
			bool operator==(const noob::mixer::voice_instance& rhs)
			{
				if (index == rhs.index) return true;
				return false;
			}
			uint32_t index;
			float volume;
		};


		struct loop_command
		{
			bool operator<(const noob::mixer::loop_command& rhs)
			{
				if (voice < rhs.voice) return true;
				if (count < 0 && rhs.count > 0) return true;
				return false; 
			}
			voice_instance voice;
			// Negative if it's not supposed to stop.
			int32_t count;
		};

		rde::vector<voice_instance> now_playing;
		rde::vector<loop_command> now_looping;

		// uint32 is the sample handle's inner member
		// rde::vector<size_t> refers to the current offset
		// rde::hash_map<voice_instance, rde::vector<size_t>> sequencer;

		rde::vector<double> buffer;

		uint32_t buffer_size;
	};
}
