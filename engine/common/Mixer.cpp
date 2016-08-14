#include "Mixer.hpp"

#include "Globals.hpp"

bool noob::mixer::play_clip(const noob::sample_handle clip, float volume) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();
	noob::audio_sample* samp = g.samples.get(clip);
	size_t min_before_next = samp->min_before_next;

	voice_instance to_search;
	to_search.index = clip.index();
	to_search.active = true;
	// rde::vector<voice_instance>::iterator it = rde::upper_bound(&now_playing[0], &now_playing[now_playing.size()-1], to_search, rde::less<voice_instance>());

	auto it = std::find(now_playing.begin(), now_playing.end(), to_search);

	if (it == now_playing.end())
	{
		(to_search.queue[0]).offset = 0;
		(to_search.queue[0]).volume = volume;
		now_playing.push_back(to_search);
		dirty = true;

		logger::log("[Mixer] Adding new sound to mixer's soundbank.");
	}
	else
	{
		it->active = true;
		for (uint32_t i = 0; i < it->queue.size(); ++i)
		{
			// Test to see if any given offset in the queue happens to be our favourite value...
			if ((it->queue[i]).offset == std::numeric_limits<size_t>::max())
			{
				(it->queue[i]).offset = 0;
				(it->queue[i]).volume = volume;

				logger::log("[Mixer] Found song in existing soundbank. Making it play again.");

				return true;
			}
			logger::log("[Mixer] Sound found in soundbank, but unable to enque as its queue is already full. Try again later.");
		}
	}

	return false;
}



void noob::mixer::tick(uint32_t num_frames) noexcept(true)
{
	if (dirty)
	{
		rde::quick_sort(&now_playing[0], &now_playing[now_playing.size()], rde::less<voice_instance>());
	}

	if (output_buffer.size() < num_frames)
	{
		output_buffer.resize(num_frames);
	}

	rde::fill_n(&output_buffer[0], output_buffer.size(), 0.0);

	noob::globals& g = noob::globals::get_instance();

	uint32_t num_voices = now_playing.size();
	for (uint32_t v = 0; v < num_voices; ++v)
	{
		if (now_playing[v].active)
		{
			noob::audio_sample* samp = g.samples.get(noob::sample_handle::make(now_playing[v].index));
			size_t sample_size = samp->samples.size();
			
			uint32_t queue_count = 0;
			
			for(uint32_t q = 0; q < 4; ++q)
			{
				noob::mixer::voice_instance::playback_info inf = now_playing[v].queue[q];
				if (inf.offset != std::numeric_limits<size_t>::max())
				{
					++queue_count;
					for (uint32_t i = 0; i < num_frames; ++i)
					{
						size_t sample_pos = inf.offset + i;
						if (sample_pos < sample_size)
						{
							double d = static_cast<double>(samp->samples[sample_pos]) * (1.0f/32768.0);
							output_buffer[i] += (d * inf.volume);
						}
					}

					inf.offset = inf.offset + num_frames;
					if (inf.offset > sample_size - 1)
					{
						inf.offset = std::numeric_limits<size_t>::max();
					}

					now_playing[v].queue[q] = inf;
				}
			}

			if (queue_count == 0)
			{
				now_playing[v].active = false;
			}
		}
	}
}
