#pragma once

typedef int (*AndroidAudioCallback)(int16_t* buffer, int num_samples);

bool opensl_wrapper_init(AndroidAudioCallback cb, int frames_per_buffer_arg, int sample_rate_arg);
void opensl_wrapper_shutdown();
