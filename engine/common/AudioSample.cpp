#include "AudioSample.hpp"
#include "Logger.hpp"
#include "Globals.hpp"

#include <stdlib.h>
#include <stdio.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include <CDSPResampler.h>

bool noob::audio_sample::load_file(const std::string& filename) noexcept(true)
{
	OggVorbis_File vf;
#ifdef _WIN32 /* We need to set stdin to binary mode under Windows */
	_setmode( _fileno( stdin ), _O_BINARY );
#endif

	logger::log("[AudioSample] About to open vorbis file...");

	// if (ov_open_callbacks(), &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	if (ov_fopen(filename.c_str(), &vf) < 0)
	{
		logger::log("Error opening vorbis file!");
	}


	vorbis_info* vi = ov_info(&vf,-1);

	num_channels = vi->channels;
	rate = vi->rate;

	char **ptr=ov_comment(&vf,-1)->user_comments;


	noob::logger::log(noob::concat("[AudioSample] Bitstream is ", noob::to_string(num_channels), " channel, ", noob::to_string(rate), "Hz. Decoded lengths: ", noob::to_string(static_cast<long>(ov_pcm_total(&vf,-1))), ". Encoded by: ", ov_comment(&vf,-1)->vendor));//". User comment: "; 
/*
	while(*ptr)
	{	
		log_msg << *ptr;
		++ptr;
	}	

	logger::log(log_msg.str());
*/

	int current_section;
	int64_t total_size = ov_pcm_total(&vf, -1);

	samples.resize(total_size);

	bool done = false;
	long accum  = 0;
	while (!done)
	{
		long return_val = ov_read(&vf, reinterpret_cast<char*>(&samples[accum/2]), total_size, 0, 2, 1, &current_section);
		accum += return_val;
		switch (return_val)
		{
			case(OV_HOLE):
				{
					done = true;
					logger::log("[AudioSample] Data interrupted while reading.");
					return false;
				}
			case(OV_EBADLINK):
				{
					logger::log("[AudioSample] Bad link exception.");
					done = true;
					return false;
				}
			case(OV_EINVAL):
				{
					logger::log("[AudioSample] Initial file headers could not be read.");
					done = true;
					return false;
				}
			case (0):
				{
					done = true;
					logger::log("[AudioSample] EOF");
					break;
				}
			default:
				{
					// fmt::MemoryWriter ww;
					// ww << "[AudioSample] " << return_val << " samples read. Current section: " << current_section;
					// logger::log(ww.str());
					break;
				}
		}

		if (!(accum < samples.size()))
		{
			done = true;
		}
	}

	logger::log(noob::concat("[AudioSample] Sample buffer size ", noob::to_string(samples.size()), ". Samples read: ", noob::to_string(accum), "."));

	noob::globals& g = noob::globals::get_instance();

	if (g.sample_rate != 44100)
	{
		size_t num_samples_old = samples.size();

		std::vector<double> old_samps;

		for (int16_t s : samples)
		{
			double d = static_cast<double>(s) / 32768.0;
			old_samps.push_back(d);
		}

		r8b::CDSPResampler24 resamp(44100.0, static_cast<double>(g.sample_rate), num_samples_old);

		size_t num_samples_new = (num_samples_old * g.sample_rate) / 44100.0;

		// samples.reserve(num_samples_new);
		samples.clear();

		size_t ol = num_samples_new;
		while (ol > 0)
		{

			double* opp;
			size_t write_count;

			double* output;
			write_count = resamp.process(&old_samps[0], num_samples_old, output);

			if (write_count > ol)
			{
				write_count = ol;
			}

			for (size_t i = 0; i < write_count; ++i)
			{
				double f = output[i] * 32768.0;
				int16_t s = static_cast<int16_t>(f);
				samples.push_back(s);
			}

			ol -= write_count;
		}

	noob::logger::log(noob::concat("[AudioSample] Resampling from 44100 to ", noob::to_string(g.sample_rate), ". Old number of samples: ", noob::to_string(num_samples_old), ". New number of samples: ", noob::to_string(num_samples_new)));


	return true;
}


bool noob::audio_sample::load_mem(const std::string& file) noexcept(true)
{

	return false;
}
