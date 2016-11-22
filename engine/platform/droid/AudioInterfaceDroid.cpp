// Minimal audio streaming using OpenSL.
//
// Loosely based on the Android NDK sample code.

#include <assert.h>
#include <string.h>
#include <unistd.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "AudioInterfaceDroid.hpp"

#include "NoobUtils.hpp"

// This is kinda ugly, but for simplicity I've left these as globals just like in the sample,
// as there's not really any use case for this where we have multiple audio devices yet.

// engine interfaces
static SLObjectItf engine_sl;
static SLEngineItf engine_engine_sl;
static SLObjectItf output_mix_sl;

// buffer queue player interfaces
static SLObjectItf bq_player_obj_sl = nullptr;
static SLPlayItf bq_player_play_sl = nullptr;
static SLAndroidSimpleBufferQueueItf bq_player_bufferqueue_sl = nullptr;
static SLMuteSoloItf bq_player_mute_solo_sl = nullptr;
static SLVolumeItf bq_player_volume = nullptr;

// Double buffering.
static short *buffer[2];
static int current_buf = 0;
static int frames_per_buffer;
static int sample_rate;

static AndroidAudioCallback audio_callback;

// This callback handler is called every time a buffer finishes playing.
// The documentation available is very unclear about how to best manage buffers.
// I've chosen to this approach: Instantly enqueue a buffer that was rendered to the last time,
// and then render the next. Hopefully it's okay to spend time in this callback after having enqueued. 
static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	if (bq != bq_player_bufferqueue_sl)
	{
		noob::logger::log(noob::importance::ERROR, "Wrong bq!");
		return;
	}

	int renderedFrames = audio_callback(buffer[current_buf], frames_per_buffer);

	int size_in_bytes = frames_per_buffer * 2 * sizeof(short);
	//int byteCount = (frames_per_buffer - renderedFrames) * 4;
	// Zero out the unplayed stuff.
	//if (byteCount > 0)
	//{
	//	memset(buffer[current_buf] + renderedFrames * 2, 0, byteCount);
	//}
	SLresult result = (*bq_player_bufferqueue_sl)->Enqueue(bq_player_bufferqueue_sl, buffer[current_buf], size_in_bytes);

	// Comment from sample code:
	// the most likely other result is SL_RESULT_BUFFER_INSUFFICIENT,
	// which for this code example would indicate a programming error
	if (result != SL_RESULT_SUCCESS)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("OpenSL ES: Failed to enqueue! ", noob::to_string(renderedFrames), " ", noob::to_string(size_in_bytes)));
	}

	current_buf ^= 1; // Switch buffer
}

// create the engine and output mix objects
bool opensl_wrapper_init(AndroidAudioCallback cb, int frames_per_buffer_arg, int sample_rate_arg)
{
	audio_callback = cb;
	frames_per_buffer = frames_per_buffer_arg;

	if (frames_per_buffer == 0)
	{
		frames_per_buffer = 256;
	}
	if (frames_per_buffer < 32)
	{
		frames_per_buffer = 32;
	}

	sample_rate = sample_rate_arg;
	if (sample_rate != 44100 && sample_rate != 48000)
	{
		noob::logger::log(noob::importance::ERROR, noob::concat("Invalid sample rate: ", noob::to_string(sample_rate), " - choosing 44100"));
		sample_rate = 44100;
	}

	buffer[0] = new short[frames_per_buffer * 2];
	buffer[1] = new short[frames_per_buffer * 2];

	SLresult result;
	// create engine
	result = slCreateEngine(&engine_sl, 0, nullptr, 0, nullptr, nullptr);
	assert(SL_RESULT_SUCCESS == result);
	result = (*engine_sl)->Realize(engine_sl, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	result = (*engine_sl)->GetInterface(engine_sl, SL_IID_ENGINE, &engine_engine_sl);
	assert(SL_RESULT_SUCCESS == result);
	result = (*engine_engine_sl)->CreateOutputMix(engine_engine_sl, &output_mix_sl, 0, nullptr, nullptr);
	assert(SL_RESULT_SUCCESS == result);
	result = (*output_mix_sl)->Realize(output_mix_sl, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);

	SLuint32 sr = SL_SAMPLINGRATE_44_1;
	if (sample_rate == 48000)
	{
		sr = SL_SAMPLINGRATE_48;
	}

	SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
	SLDataFormat_PCM format_pcm =
	{
		SL_DATAFORMAT_PCM,
		2,
		sr,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_PCMSAMPLEFORMAT_FIXED_16,
		SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
		SL_BYTEORDER_LITTLEENDIAN
	};

	SLDataSource audioSrc = {&loc_bufq, &format_pcm};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, output_mix_sl};
	SLDataSink audioSnk = {&loc_outmix, nullptr};

	// create audio player
	const SLInterfaceID ids[2] = {SL_IID_BUFFERQUEUE, SL_IID_VOLUME};
	const SLboolean req[2] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};
	result = (*engine_engine_sl)->CreateAudioPlayer(engine_engine_sl, &bq_player_obj_sl, &audioSrc, &audioSnk, 2, ids, req);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_obj_sl)->Realize(bq_player_obj_sl, SL_BOOLEAN_FALSE);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_PLAY, &bq_player_play_sl);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_BUFFERQUEUE, &bq_player_bufferqueue_sl);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_bufferqueue_sl)->RegisterCallback(bq_player_bufferqueue_sl, bqPlayerCallback, nullptr);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_VOLUME, &bq_player_volume);
	assert(SL_RESULT_SUCCESS == result);
	result = (*bq_player_play_sl)->SetPlayState(bq_player_play_sl, SL_PLAYSTATE_PLAYING);
	assert(SL_RESULT_SUCCESS == result);

	// Render and enqueue a first buffer.
	current_buf = 0;

	audio_callback(buffer[current_buf], frames_per_buffer);

	int size_in_bytes = frames_per_buffer * 2 * sizeof(short);

	result = (*bq_player_bufferqueue_sl)->Enqueue(bq_player_bufferqueue_sl, buffer[current_buf], size_in_bytes);

	if (SL_RESULT_SUCCESS != result)
	{
		return false;
	}

	current_buf ^= 1;

	return true;
}

// shut down the native audio system
void opensl_wrapper_shutdown()
{
	SLresult result;
	noob::logger::log(noob::importance::INFO, "opensl_wrapper_shutdown - stopping playback");
	result = (*bq_player_play_sl)->SetPlayState(bq_player_play_sl, SL_PLAYSTATE_STOPPED);
	if (SL_RESULT_SUCCESS != result)
	{
		noob::logger::log(noob::importance::ERROR, "SetPlayState failed");
	}

	noob::logger::log(noob::importance::INFO, "opensl_wrapper_shutdown - deleting player object");

	if (bq_player_obj_sl != nullptr)
	{
		(*bq_player_obj_sl)->Destroy(bq_player_obj_sl);
		bq_player_obj_sl = nullptr;
		bq_player_play_sl = nullptr;
		bq_player_bufferqueue_sl = nullptr;
		bq_player_mute_solo_sl = nullptr;
		bq_player_volume = nullptr;
	}

	noob::logger::log(noob::importance::INFO, "opensl_wrapper_shutdown - deleting mix object");

	if (output_mix_sl != nullptr)
	{
		(*output_mix_sl)->Destroy(output_mix_sl);
		output_mix_sl = nullptr;
	}

	noob::logger::log(noob::importance::INFO, "opensl_wrapper_shutdown - deleting engine object");

	if (engine_sl != nullptr)
	{
		(*engine_sl)->Destroy(engine_sl);
		engine_sl = nullptr;
		engine_engine_sl = nullptr;
	}
	delete [] buffer[0];
	delete [] buffer[1];
	noob::logger::log(noob::importance::INFO, "opensl_wrapper_shutdown - finished");
}	


