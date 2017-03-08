/*
Adapted from code originally found at:
https://github.com/hrydgard/native/blob/master/android/native-audio-so.cpp

Upstream license:
Copyright (C) 2012 Henrik Rydgard

This applies to all the code here not covered under other licenses, see README.md.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 

// Minimal audio streaming using OpenSL.
//
// Loosely based on the Android NDK sample code.

#include <string.h>
#include <unistd.h>

// for native audio
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

#include "AudioInterfaceDroid.hpp"

#include "NoobUtils.hpp"

// Engine interfaces
static SLObjectItf engine_sl;
static SLEngineItf engine_engine_sl;
static SLObjectItf output_mix_sl;

// Buffer queue player interfaces
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
static void bqPlayerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)
{
	if (bq != bq_player_bufferqueue_sl)
	{
		noob::logger::log(noob::importance::ERROR, "Wrong bq!");
		return;
	}

	uint32_t size_in_bytes = frames_per_buffer * 2 * sizeof(int16_t);

	uint32_t renderedFrames = audio_callback(buffer[current_buf], frames_per_buffer);

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
		noob::logger::log(noob::importance::ERROR, noob::concat("OpenSL ES: Failed to enqueue buffah!"));
	}


	current_buf ^= 1; // Switch buffer
}

// create the engine and output mix objects
bool opensl_wrapper_init(AndroidAudioCallback cb, uint32_t frames_per_buffer_arg, uint32_t sample_rate_arg)
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
	if (SL_RESULT_SUCCESS != result) return false;
	result = (*engine_sl)->Realize(engine_sl, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*engine_sl)->GetInterface(engine_sl, SL_IID_ENGINE, &engine_engine_sl);
	if (SL_RESULT_SUCCESS != result) return false;
	result = (*engine_engine_sl)->CreateOutputMix(engine_engine_sl, &output_mix_sl, 0, nullptr, nullptr);
	if (SL_RESULT_SUCCESS != result) return false;
	result = (*output_mix_sl)->Realize(output_mix_sl, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) return false;

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
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_obj_sl)->Realize(bq_player_obj_sl, SL_BOOLEAN_FALSE);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_PLAY, &bq_player_play_sl);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_BUFFERQUEUE, &bq_player_bufferqueue_sl);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_bufferqueue_sl)->RegisterCallback(bq_player_bufferqueue_sl, bqPlayerCallback, nullptr);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_obj_sl)->GetInterface(bq_player_obj_sl, SL_IID_VOLUME, &bq_player_volume);
	if (SL_RESULT_SUCCESS != result) return false;	
	result = (*bq_player_play_sl)->SetPlayState(bq_player_play_sl, SL_PLAYSTATE_PLAYING);
	if (SL_RESULT_SUCCESS != result) return false;


	// Render and enqueue a first buffer.
	current_buf = 0;

	audio_callback(buffer[current_buf], frames_per_buffer);

	int size_in_bytes = frames_per_buffer * 2 * sizeof(int16_t);

	result = (*bq_player_bufferqueue_sl)->Enqueue(bq_player_bufferqueue_sl, buffer[current_buf], size_in_bytes);

	if (SL_RESULT_SUCCESS != result) return false;

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


