#pragma once
 
 // std
#include <cstdint>
#include <vector>
#include <limits>

// External libs
#include <rdestl/algorithm.h>
#include <rdestl/basic_string.h>
#include <rdestl/fixed_array.h>
#include <rdestl/hash_map.h>
#include <noob/component/component.hpp>

// Project headers
#include "AudioSample.hpp"
#include "RingBuffer.hpp"

namespace noob
{
	class mixer
	{
		public:

		mixer() noexcept(true) : dirty(false), num_playing(0), max_playing(8), output_buffer_size(0) {}

		// Returns false for three reasons:
		// First: Invalid handle (ie: sample doesn't exist.)
		// Second: Not enough free voices.
		// Third: Trying to play sample prior to its minimum allowed offset being reached (each sample has a minimum offset to ensure two clips being played at the same time don't cause interference.
		bool play_clip(noob::audio_sample_handle, float volume) noexcept(true);
		
		void tick(uint32_t num_frames) noexcept(true);

		std::vector<float> output_buffer;

		protected:

		struct voice
		{
			voice() noexcept(true) : active(true), index(0) {}

			bool operator<(const noob::mixer::voice& rhs) const noexcept(true);
			bool operator==(const noob::mixer::voice& rhs) const noexcept(true);
			
			struct instance
			{
				instance() noexcept(true) : offset(std::numeric_limits<size_t>::max()), volume(0.0) {}

				bool operator<(const noob::mixer::voice::instance& rhs) const noexcept(true);
				bool operator==(const noob::mixer::voice::instance& rhs) const noexcept(true);

				size_t offset;
				float volume;
			};

			bool active;
			uint32_t index;
			// rde::fixed_array<noob::mixer::voice::instance, 4> queue;
			std::array<voice::instance, 4> queue;
		};

		bool dirty;
		uint32_t num_playing, max_playing, output_buffer_size;
		//rde::vector<voice> voices;
		std::vector<voice> voices;
	};
}
