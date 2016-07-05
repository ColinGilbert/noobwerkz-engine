#include "AudioSample.hpp"
#include "Logger.hpp"

#include <stdlib.h>
#include <stdio.h>

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>


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


	fmt::MemoryWriter log_msg;
	log_msg << "[AudioSample] Bitstream is " << num_channels << " channel, " << rate << "Hz. Decoded lengths: " << static_cast<long>(ov_pcm_total(&vf,-1)) << ". Encoded by: " << ov_comment(&vf,-1)->vendor << ". User comment: "; 
	while(*ptr)
	{	
		log_msg << *ptr;
		++ptr;
	}	

	logger::log(log_msg.str());


	int current_section;
	int64_t total_size = ov_pcm_total(&vf, -1);

	samples.resize(total_size);

	// Holy crap blame Ogg Vorbis for that C++-to-triple-pointer-C hack and not Colin...
	float* ptr_to_samples = &samples[0];
	float** ptr_to_ptr_to_samples = &ptr_to_samples;

	bool done = false;
	long accum  = 0;
	while (!done)
	{
		long return_val = ov_read_float(&vf, &ptr_to_ptr_to_samples, total_size, &current_section);
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
					fmt::MemoryWriter ww;
					ww << "[AudioSample] " << return_val << " samples read.";
					logger::log(ww.str());
					break;
				}
		}

		if (!(accum < samples.size()))
		{
			done = true;
		}
	}
	fmt::MemoryWriter ww;
	ww << "[AudioSample] Sample buffer size " << samples.size() << ". Accumulated values: " << accum << ".";
	logger::log(ww.str());

	return true;
}


bool noob::audio_sample::load_mem(const std::string& file) noexcept(true)
{

	return false;
}
