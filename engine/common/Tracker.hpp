#pragma once

#include <vector>
#include <array>
#include <limits>

#include "ComponentDefines.hpp"
#include "AudioSample.hpp"

namespace noob
{
	class tracker
	{
		friend class mixer;
		public:

			tracker() noexcept(true) : dirty(false), num_playing(0), max_playing(8) {}

			// Returns false for three reasons:
			// First: Invalid handle (ie: sample doesn't exist.)
			// Second: Not enough free voices.
			// Third: Trying to play sample prior to its minimum allowed offset being reached (each sample has a minimum offset to ensure two clips being played at the same time don't cause interference.
			bool add(noob::sample_handle, float volume) noexcept(true);

			void tick(uint32_t num_frames) noexcept(true);

		protected:

			struct voice
			{
				voice() noexcept(true) : active(true), index(0) {}

				bool operator<(const noob::tracker::voice& rhs) const noexcept(true);
				bool operator==(const noob::tracker::voice& rhs) const noexcept(true);

				struct instance
				{
					instance() noexcept(true) : offset(std::numeric_limits<size_t>::max()), volume(0.0) {}

					bool operator<(const noob::tracker::voice::instance& rhs) const noexcept(true);
					bool operator==(const noob::tracker::voice::instance& rhs) const noexcept(true);

					size_t offset;
					float volume;
				};

				bool active;
				uint32_t index;
				// rde::fixed_array<noob::tracker::voice::instance, 4> queue;
				std::array<voice::instance, 4> queue;
			};

			bool dirty;
			uint32_t num_playing, max_playing, output_buffer_size;
			//rde::vector<voice> voices;
			std::vector<voice> voices;
	};
}
