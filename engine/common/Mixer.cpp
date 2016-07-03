#include "Mixer.hpp"


bool noob::mixer::play_clip(const noob::sample_handle clip, float volume) noexcept(true)
{
	if (num_playing < max_playing)
	{
		noob::globals& g = noob::globals::get_instance();

		bool doit = false;
		noob::audio_sample* samp = g.samples.get(clip);
		size_t min_before_next = samp->min_before_next;
		voice_instance to_search;
		to_search.index = clip.get_inner();
		to_search.active = true;
		to_search.offset = 0;
		rde::vector<voice_instance>::iterator it = rde::lower_bound(&now_playing[0], &now_playing[now_playing.size()], to_search, rde::less<voice_instance>());

		while (it != now_playing.end())
		{
			voice_instance current_v = *it;

			if (current_v.index > it->index) 
			{
				doit = true;
				break;
			}
			if (current_v.index < it->index)
			{
				continue;
			}
			if (current_v.index == it->index && min_before_next < (it->offset))
			{
				doit = true;
				break;
			}
			++it;
		}

		if (doit)
		{
			voice_instance v;
			v.active = true;
			v.index = clip.get_inner();
			v.offset = 0;
			v.volume = volume;
			now_playing[num_playing] = v;
		}

		++num_playing;
		dirty = true;
	}
}


void noob::mixer::tick(uint32_t num_frames) noexcept(true)
{
	if (num_frames > output_buffer.size())
	{
		output_buffer.resize(num_frames);
	}


	if (dirty)
	{
		rde::quick_sort(&now_playing[0], &now_playing[now_playing.size()], rde::less<voice_instance>());
	}

	noob::globals& g = noob::globals::get_instance();

	// Mix samples to output buffer and flip the finished ones to inactive.
	for (uint32_t incr = 0; incr < num_playing; ++incr)
	{
		voice_instance curr_sample = now_playing[incr];
		noob::audio_sample* samp = g.samples.get(noob::sample_handle::make(curr_sample.index));
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
