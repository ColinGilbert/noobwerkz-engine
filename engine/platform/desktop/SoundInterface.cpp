#include "SoundInterface.hpp"

#include <soundio/soundio.h>

#include "NoobUtils.hpp"
#include "Globals.hpp"


static struct SoundIo* soundio;
static struct SoundIoDevice* device;
static struct SoundIoOutStream* outstream;

static void write_callback(struct SoundIoOutStream *outstream, int frame_count_min, int frame_count_max)
{
	const struct SoundIoChannelLayout* layout = &outstream->layout;
	double float_sample_rate = outstream->sample_rate;
	double seconds_per_frame = 1.0f / float_sample_rate;
	struct SoundIoChannelArea *areas;
	int frames_left = frame_count_max;
	int err;
	
	noob::globals& g = noob::globals::get_instance();

	while (frames_left > 0)
	{
		int frame_count = frames_left;

		if ((err = soundio_outstream_begin_write(outstream, &areas, &frame_count)))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Sound] SoundIO error at beginning of frame ", noob::to_string(frame_count) ," - ", soundio_strerror(err)));
			return;
		}

		if (!frame_count)
		{
			break;
		}
		
		g.master_mixer.tick(frame_count);

		for (int frame = 0; frame < frame_count; ++frame)
		{
			float sample = g.master_mixer.output_buffer[frame];
			for (int channel = 0; channel < layout->channel_count; ++channel)
			{
				float *ptr = (float*)(areas[channel].ptr + areas[channel].step * frame);
				*ptr = sample;
			}
		}

		if ((err = soundio_outstream_end_write(outstream)))
		{
			noob::logger::log(noob::importance::ERROR, noob::concat("[Sound] SoundIO error at end of frame ", noob::to_string(frame_count), " - ", soundio_strerror(err)));
			return;
		}

		frames_left -= frame_count;
	}
}

void noob::sound_interface::init()
{
	int err;

	struct SoundIo* soundio = soundio_create();

	if (!soundio)
	{
		noob::logger::log(noob::importance::ERROR, "[Sound] ERROR: Couldn't even start SoundIO!");
		return;
	}

	if ((err = soundio_connect(soundio)))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Sound] ERROR: Error connecting SoundIO - ", soundio_strerror(err)));
		return;
	}

	soundio_flush_events(soundio);

	int default_out_device_index = soundio_default_output_device_index(soundio);

	if (default_out_device_index < 0)
	{
		noob::logger::log(noob::importance::ERROR, "[Sound] ERROR: No SoundIO output device found!");
		return;
	}

	device = soundio_get_output_device(soundio, default_out_device_index);

	if (!device)
	{
		noob::logger::log(noob::importance::ERROR, "[Sound] ERROR: Cannot get SoundIO output device!");
		return;
	}
	else
	{
		noob::logger::log(noob::importance::INFO, noob::concat("[Sound] Init: Got SoundIO output device - ", device->name));
	}
	outstream = soundio_outstream_create(device);

	outstream->format = SoundIoFormatFloat32NE;
	outstream->write_callback = write_callback;

	if ((err = soundio_outstream_open(outstream)))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Sound] ERROR: Unable to open SoundIO device - ", soundio_strerror(err)));
		return;
	}

	if (outstream->layout_error)
	{
		noob::logger::log(noob::importance::WARNING, noob::concat("[Sound] WARNING: Unable to set SoundIO channel layout - ", soundio_strerror(outstream->layout_error)));
	}


	if ((err = soundio_outstream_start(outstream)))
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("[Sound] ERROR: Unable to start SoundIO outstream - ", soundio_strerror(err)));
		return;
	}

	noob::globals& g = noob::globals::get_instance();
	g.sample_rate = outstream->sample_rate;

	noob::logger::log(noob::importance::INFO, noob::concat("[SoundInterface] Sound init success! Sample rate: ", noob::to_string(outstream->sample_rate), ". Bytes per frame: ", noob::to_string(outstream->bytes_per_frame), ". Bytes per sample: ", noob::to_string(outstream->bytes_per_sample)));

	valid = true;
}

void noob::sound_interface::run()
{
	for (;;)
	{
		if (valid)
		{
			soundio_wait_events(soundio);
		}
	}
}

void noob::sound_interface::tear_down()
{
	valid = false;
	soundio_outstream_destroy(outstream);
	soundio_device_unref(device);
	soundio_destroy(soundio);
}
