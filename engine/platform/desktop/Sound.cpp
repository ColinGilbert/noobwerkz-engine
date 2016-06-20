#include "Sound.hpp"
#include "Logger.hpp"

#include <soundio/soundio.h>

#define NOOB_PI 3.1415926535

static float seconds_offset = 0.0f;
static struct SoundIo* soundio;
static struct SoundIoDevice* device;
static struct SoundIoOutStream* outstream;

static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max)
{
	const struct SoundIoChannelLayout* layout = &outstream->layout;
	float float_sample_rate = outstream->sample_rate;
	float seconds_per_frame = 1.0f / float_sample_rate;
	struct SoundIoChannelArea *areas;
	int frames_left = frame_count_max;
	int err;

	while (frames_left > 0)
	{
		int frame_count = frames_left;

		if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
		{
			fmt::MemoryWriter ww;
			ww << "[Sound] SoundIO error at beginning of frame " << frame_count << " - " << soundio_strerror(err);
			noob::logger::log(ww.str());
			return;
		}

		if (!frame_count)
		{
			break;
		}

		float pitch = 440.0f;
		float radians_per_second = pitch * 2.0f * NOOB_PI;

		for (int frame = 0; frame < frame_count; frame += 1)
		{
			float sample = sinf((seconds_offset + frame * seconds_per_frame) * radians_per_second);
			for (int channel = 0; channel < layout->channel_count; channel += 1)
			{
				float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
				*ptr = sample;
			}
		}

		seconds_offset += seconds_per_frame * frame_count;

		if ((err = soundio_outstream_end_write(outstream)))
		{
			fmt::MemoryWriter ww;
			ww << "[Sound] SoundIO error at end of frame " << frame_count << " - " << soundio_strerror(err);
			noob::logger::log(ww.str());
			return;
		}

		frames_left -= frame_count;
	}
}

void noob::sound::init()
{
	int err;

	struct SoundIo* soundio = soundio_create();

	if (!soundio)
	{
		noob::logger::log("[Sound] Init: Error starting SoundIO.");
		return false;
	}

	if ((err = soundio_connect(soundio)))
	{
		fmt::MemoryWriter ww;
		ww << "[Sound] Init: Error connecting with SoundIO - " << soundio_strerror(err);
		noob::logger::log(ww.str());
		return false;
	}

	soundio_flush_events(soundio);

	int default_out_device_index = soundio_default_output_device_index(soundio);

	if (default_out_device_index < 0)
	{
		noob::logger::log("[Sound] Init: No SoundIO output device found.");
		return false;
	}

	device = soundio_get_output_device(soundio, default_out_device_index);

	if (!device)
	{
		noob::logger::log("[Sound] Init: Cannot get SoundIO output device!");
		return false;
	}

	{
		fmt::MemoryWriter ww;
		ww << "[Sound] Init: Got SoundIO output device - " << device->name;
		noob::logger::log(ww.str());
	}

	outstream = soundio_outstream_create(device);

	outstream->format = SoundIoFormatU16LE;
	outstream->write_callback = write_callback;

	if ((err = soundio_outstream_open(outstream)))
	{
		fmt::MemoryWriter ww;
		ww << "[Sound] Error: Unable to open SoundIO device" << soundio_strerror(err);
		noob::logger::log(ww.str());
		return false;
	}

	if (outstream->layout_error)
	{
		fmt::MemoryWriter ww;

		ww << "[Sound] Error: Unable to set SoundIO channel layout - " << soundio_strerror(outstream->layout_error) << " - Trying different configs.";
		noob::logger::log(ww.str());
		
		
		
		
		//return false;
	}


	if ((err = soundio_outstream_start(outstream)))
	{
		fmt::MemoryWriter ww;
		ww << "[Sound] Error: Unable to start SoundIO outstream - " <<  soundio_strerror(err);
		noob::logger::log(ww.str());
		return false;
	}

	valid = true;
}


void noob::sound::run()
{
	if (valid)
	{
		for (;;)
		{
			soundio_wait_events(soundio);
		}
	}
}

void noob::sound::tear_down()
{
	valid = false;
	soundio_outstream_destroy(outstream);
	soundio_device_unref(device);
	soundio_destroy(soundio);
}

void noob::sound::play(const std::vector<uint16_t>& sample)
{
	
}
