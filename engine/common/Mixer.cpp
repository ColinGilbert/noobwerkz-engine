#include "Mixer.hpp"


bool noob::mixer::play_clip(const noob::sample_handle add_clip, float volume)
{
}


void noob::mixer::tick(uint32_t num_frames)
{
	// build_playlist();

/*
	if (num_frames > buffer.size())
	{
		buffer.resize(num_frames);
	}

	noob::globals& g = noob::globals::get_instance();

	rde::vector<rde::pair<voice_instance, uint32_t>> deletions;

	for (voice_instance current_voice : now_playing)
	{
		uint32_t num_deletions = 0;
		noob::audio_sample* sample_ptr = g.samples.get(sample_handle::make(current_voice.index));
		size_t sample_max_frame = sample_ptr->samples.size();
		auto it = sequencer.find(current_voice);
		if (it != sequencer.end())
		{
			for (uint32_t i = 0; i < (it->second).size(); ++i)
			{
				size_t sample_current_frame = (it->second)[i];

				// Pick up frame from samples pool and mix into buffer here:


				// Clips that have finished playing are marked for removal.
				if (sample_current_frame = sample_max_frame)
				{
					++num_deletions;
					(it->second)[i] = std::numeric_limits<size_t>::max();
				}
			}
			if (num_deletions > 0)
			{
				deletions.push_back(rde::make_pair(current_voice, num_deletions));
			}
		}
		else
		{
			logger::log("[Mixer] ERROR! Item in now_playing not found in sequencer. This is a bug!");
		}
	}


	// Remove finished clips...
	for (rde::pair<voice_instance, uint32_t> current : deletions)
	{
		auto it = sequencer.find(current.first);

		if (it != sequencer.end())
		{
			rde::vector<size_t> v(it->second);
			rde::quick_sort(&v[0], &v[v.size()]);
			uint32_t new_size = v.size() - current.second;
			v.resize(new_size);

			sequencer.erase(current.first);
			sequencer.insert(rde::make_pair(current.first, v));
		}
		else
		{
			logger::log("[Mixer] Item that was literally there during the same function ago not present anymore. Threading error!");
		}
	}

	*/
}


void noob::mixer::build_playlist()
{
/*
	for (loop_command current_loop : now_looping)
	{
		auto it = sequencer.find(current_loop.voice);
		if (it != sequencer.end())
		{
			// Find out it last item in vector is far enough away to add to loop again.
		}
	}
*/
}
