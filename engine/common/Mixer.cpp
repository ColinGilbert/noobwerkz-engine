// Header file we're implementing
#include "Mixer.hpp"

// Project headers
#include "Globals.hpp"
#include "NoobUtils.hpp"


bool noob::mixer::play_clip(const noob::audio_sample_handle clip, float volume) noexcept(true)
{
	noob::globals& g = noob::get_globals();
	noob::audio_sample* samp = g.samples.get(clip);
	size_t min_before_next = samp->min_before_next;

	voice to_search;
	to_search.index = clip.index();
	to_search.active = true;
	// rde::vector<voice>::iterator it = rde::upper_bound(&voices[0], &voices[voices.size()-1], to_search, rde::less<voice>());

	auto it = std::find(voices.begin(), voices.end(), to_search);

	if (it == voices.end())
	{
		(to_search.queue[0]).offset = 0;
		(to_search.queue[0]).volume = volume;
		voices.push_back(to_search);
		dirty = true;

		logger::log(noob::importance::INFO, "[Mixer] Adding new sound to mixer's soundbank.");
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

				// logger::log(noob::importance::INFO, "[Mixer] Found song in existing soundbank. Making it play again.");

				return true;
			}
			// logger::log(noob::importance::INFO, "[Mixer] Sound found in soundbank, but unable to enqueue as its queue is already full. Try again later.");
		}
	}

	return false;
}


void noob::mixer::tick(uint32_t num_frames) noexcept(true)
{
	if (dirty)
	{
		rde::quick_sort(&voices[0], &voices[voices.size()], rde::less<voice>());
	}

	if (output_buffer.size() < num_frames)
	{
		output_buffer.resize(num_frames);
	}

	rde::fill_n(&output_buffer[0], output_buffer.size(), 0.0f);

	noob::globals& g = noob::get_globals();

	const size_t num_voices = voices.size();
	for (size_t v = 0; v < num_voices; ++v)
	{
		if (voices[v].active)
		{
			const noob::audio_sample* samp = g.samples.get(noob::audio_sample_handle::make(voices[v].index));
			const size_t sample_size = samp->samples.size();
			for(size_t q = 0; q < 4; ++q)
			{
				noob::mixer::voice::instance inf = voices[v].queue[q];
				if (inf.offset != std::numeric_limits<size_t>::max())
				{
					for (size_t i = 0; i < num_frames; ++i)
					{
						const size_t sample_pos = inf.offset + i;
						if (sample_pos < sample_size)
						{
							const float f = static_cast<float>(samp->samples[sample_pos]) * (1.0f/32768.0);
							output_buffer[i] += (f * inf.volume);
						}
					}

					inf.offset = inf.offset + num_frames;
					if (inf.offset > sample_size - 1)
					{
						inf.offset = std::numeric_limits<size_t>::max();
						voices[v].active = false;

					}

					voices[v].queue[q] = inf;
				}
			}
		}
	}
}


bool noob::mixer::voice::operator<(const noob::mixer::voice& rhs) const noexcept(true)
{
	if (index < rhs.index) return true;
	return false; 
}


bool noob::mixer::voice::operator==(const noob::mixer::voice& rhs) const noexcept(true)
{	
	if (index == rhs.index) return true;
	return false;
}


bool noob::mixer::voice::instance::operator<(const noob::mixer::voice::instance& rhs) const noexcept(true)
{
	if (offset < rhs.offset) return true;
	return false; 
}


bool noob::mixer::voice::instance::operator==(const noob::mixer::voice::instance& rhs) const noexcept(true)
{	
	if (offset == rhs.offset) return true;
	return false;
}


