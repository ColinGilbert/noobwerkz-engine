#include "Mixer.hpp"

#include "Globals.hpp"

bool noob::mixer::play_clip(const noob::sample_handle clip, float volume) noexcept(true)
{
	noob::globals& g = noob::globals::get_instance();
	noob::audio_sample* samp = g.samples.get(clip);
	size_t min_before_next = samp->min_before_next;
	voice_instance to_search;
	to_search.index = clip.get_inner();
	to_search.active = true;
	rde::vector<voice_instance>::iterator it = rde::upper_bound(&now_playing[0], &now_playing[now_playing.size()], to_search, rde::less<voice_instance>());

   	if (it == now_playing.end())
	{
		(to_search.queue[0]).offset = 0;
		(to_search.queue[0]).volume = volume;
		now_playing.push_back(to_search);
		dirty = true;

		logger::log("[Mixer] Adding new sound to mixer's soundbank,");
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


}

/*
// logger::log("[Mixer] tick()");
if (num_frames > output_buffer.size())
{
// logger::log("[Mixer] resizing buffer");
output_buffer.resize(num_frames);
}

if (dirty)
{
// logger::log("[Mixer] Sorting prior to playback (start.)");
rde::quick_sort(&now_playing[0], &now_playing[now_playing.size()], rde::less<voice_instance>());
// logger::log("[Mixer] Sorting prior to playback (done.)");
}

noob::globals& g = noob::globals::get_instance();

// logger::log("[Mixer] Got globals");

// Mix samples to output buffer and flip the finished ones to inactive.
for (uint32_t incr = 0; incr < num_playing; ++incr)
{
fmt::MemoryWriter ww;
ww << "[Mixer] Starting mix " << incr;
logger::log(ww.str());
voice_instance curr_sample = now_playing[incr];
logger::log("[Mixer] Got voice instance.");
noob::audio_sample* samp = g.samples.get(noob::sample_handle::make(curr_sample.index));
logger::log("[Mixer] Got sample.");
size_t sample_size = samp->samples.size();
// if (v.offset > sample_size) 
// {
//	v.active = false;
// v.offset = 0;
//	break;
// }
size_t frames_until_end = sample_size - curr_sample.offset;

if (num_frames < frames_until_end)
{
// Drop num_playing into the output buffer and substract the offset by num_playing

for (uint32_t i = 0; i < num_frames; ++i)
{
output_buffer[curr_sample.offset+i] += samp->samples[curr_sample.offset+i] * curr_sample.volume;
}

curr_sample.offset -= num_frames;
}
else
{
size_t frames_to_render = num_frames - frames_until_end;
// Drop frames_to_render samples into the output buffer and mark the voice as inactive

for (uint32_t i = 0; i < frames_to_render; ++i)
{
output_buffer[curr_sample.offset+i] += samp->samples[curr_sample.offset+i] * curr_sample.volume;
}

curr_sample.active = false;
curr_sample.offset = 0;
--num_playing;
}
}

// Remove finished clips by sorting up to the top...
rde::quick_sort(&now_playing[0], &now_playing[now_playing.size()], rde::less<voice_instance>());
}
*/

