#include "AudioSample.hpp"
#include <thread>

#include <stdlib.h>
#include <stdio.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <CDSPResampler.h>

#include "Globals.hpp"
#include "NoobUtils.hpp"

bool noob::audio_sample::load_file(const std::string& filename) noexcept(true)
{
	OggVorbis_File vf;
#ifdef _WIN32 /* We need to set stdin to binary mode under Windows */
	_setmode( _fileno( stdin ), _O_BINARY );
#endif

	logger::log(noob::importance::INFO, noob::concat("[AudioSample] About to open vorbis file ", filename));

	// if (ov_open_callbacks(), &vf, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0)
	if (ov_fopen(filename.c_str(), &vf) < 0)
	{
		logger::log(noob::importance::ERROR, noob::concat("[AudioSample] Error opening vorbis file!", filename));
	}

	vorbis_info* vi = ov_info(&vf,-1);

	num_channels = vi->channels;
	sample_rate = vi->rate;

	char **ptr=ov_comment(&vf,-1)->user_comments;

	noob::logger::log(noob::importance::INFO, noob::concat("[AudioSample] Bitstream is ", noob::to_string(num_channels), " channel, ", noob::to_string(sample_rate), "Hz. Decoded lengths: ", noob::to_string(static_cast<int64_t>(ov_pcm_total(&vf,-1))), ". Encoded by: ", ov_comment(&vf,-1)->vendor));//". User comment: "; 
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
					logger::log(noob::importance::ERROR, "[AudioSample] Data interrupted while reading.");
					return false;
				}
			case(OV_EBADLINK):
				{
					logger::log(noob::importance::ERROR, "[AudioSample] Bad link exception.");
					done = true;
					return false;
				}
			case(OV_EINVAL):
				{
					logger::log(noob::importance::ERROR, "[AudioSample] Initial file headers could not be read.");
					done = true;
					return false;
				}
			case (0):
				{
					done = true;
					logger::log(noob::importance::INFO, "[AudioSample] EOF");
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

	noob::logger::log(noob::importance::INFO, noob::concat("[AudioSample] Sample buffer size ", noob::to_string(samples.size()), ". Samples read: ", noob::to_string(static_cast<int64_t>(accum)), "."));

	noob::globals& g = noob::globals::get_instance();

	if (g.sample_rate != 44100)
	{
		std::thread t([this]()
		{
			noob::globals& g = noob::globals::get_instance();
			resample(g.sample_rate);
		});

		t.join();

	}
	return true;
}


void noob::audio_sample::resample(size_t sample_rate_arg) noexcept(true)
{
		size_t num_samples_old = samples.size();
		size_t sample_rate_old = sample_rate;
		sample_rate = sample_rate_arg;
		std::vector<double> old_samps;

		for (int16_t s : samples)
		{
			const double val = static_cast<float>(s) / 32768.0;
			old_samps.push_back(val);
		}

		r8b::CDSPResampler24 resamp(sample_rate_old, sample_rate, num_samples_old);
		
		samples.clear();

		const size_t num_resampled = static_cast<size_t>(static_cast<double>(num_samples_old) * (static_cast<double>(sample_rate) / static_cast<double>(sample_rate_old)));

		size_t output_left = num_resampled;
		while (output_left > 0)
		{
			double* output;
			uint32_t write_count = 0;

			write_count = resamp.process(&old_samps[0], num_samples_old, output);

			if (write_count > output_left)
			{
				write_count = output_left;
			}

			for (size_t i = 0; i < write_count; ++i)
			{
				const double f = output[i] * 32767.0;
				const int16_t s = static_cast<int16_t>(f);
				samples.push_back(s);
			}

			output_left -= write_count;
		}
	
		noob::logger::log(noob::importance::INFO, noob::concat("[AudioSample] Resampling from ", noob::to_string(sample_rate_old), " to ", noob::to_string(sample_rate), ". Old number of samples: ", noob::to_string(num_samples_old), ". New number of samples: ", noob::to_string(num_resampled)));
}
