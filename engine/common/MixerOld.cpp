#include "Mixer.hpp"


void noob::mixer::add_clip(const std::vector<uint16_t>& samples, const std::string& name)
{

	if (samples.size() > 0)
	{
		size_t start = pool.size();
		pool.reserve(start + samples.size());
		rde::fill_n<uint16_t>(&pool[start], samples.size());
		offsets.push_back(start);
		lengths.push_back(samples.size());

		assert(offset.size() == lengths.size());

		uint32_t index = offsets.size();

		names.insert(rde::make_pair(name.c_str(), index));
	}
}


void noob::mixer::resize_buffer(size_t s)
{
	buffer.resize(s);
}


void noob::mixer::play_clip(uint32_t index)
{
	// fresh_clips.push_back(index);
}


void noob::mixer::build_playlist()
{
	uint32_t temp_clip, clip_count;
	uint32_t temp_clip = std::numeric_limits<uint32_t>::max();
	uint32_t clip_count = 0;
	for (uint32_t c : fresh_clips)
	{
		if (temp_clip != c])
		{
			if (clip_count != 0)
			{
				auto it = now_playing.find(temp_clip);
				bool found = false;
				if (it != now_playing.end())
				{
					found = true;
					rde::vector<size_t> v(it->second);
					for (uint32_t cc = 0; cc < clip_count; ++cc)
					{
						v.push_back(offsets[temp_clip]);
					}
					rde::quick_sort<size_t>(v);
					now_playing.erase(temp_clip);
					now_playing.insert(rde::make_pair(temp_clip, v));
				}
			}
			temp_clip = fresh_clips[i];
			clip_count = 0;
		}
		else
		{
			++clip_count;
		}
	}
	fresh_clips.resize(0);
}


void noob::mixer::tick(uint32_t num_frames);
{
	build_playlist();

	if (num_frames > buffer.size())
	{
		buffer.resize(num_frames);
	}

	buffer_pos = 0;
	for (uint32_t n = 0; n < num_frames; ++n)
	{
		for (uint32_t clip = 0; clip < offsets.size(); ++clip)
		{
			uint32_t num_deletions = 0;
			auto it = now_playing.find(clip);
			if (it != now_playing.end())
			{
				size_t clip_first_frame = offsets[clip];
				size_t clip_last_frame = clip_first_frame + lengths[clip];

				for (uint32_t i = 0; i < (it->second).size(); ++i)
				{
					size_t clip_current_frame = clip_first_frame + (it->second)[i];

					// Pick up frame from samples pool and mix into buffer here:


					// Clips that have finished playing are marked for removal.
					if (clip_last_frame == clip_current_frame)
					{
						++num_deletions;
						(it->second)[i] = std::numeric_limits<size_t>::max();
					}
				}
			}

			// Remove finished clips...
			if (num_deletions != 0)
			{
				rde::vector<size_t> v(it->second);
				rde::quicksort<size_t>(v);
				uint32_t new_size = v.size() - num_deletions;
				v.resize(new_size);

				now_playing.erase(clip);
				now_playing.insert(rde::make_pair(clip, v));
			}
		}
		// Next frame :)
		++buffer_pos;
	}
}
