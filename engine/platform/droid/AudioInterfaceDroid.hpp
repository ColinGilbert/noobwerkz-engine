/*
Adapted from code originally found at:

https://github.com/hrydgard/native/blob/master/android/native-audio-so.h
Upstream license:

Copyright (C) 2012 Henrik Rydgard

This applies to all the code here not covered under other licenses, see README.md.

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ 


#pragma once
typedef uint32_t (*AndroidAudioCallback)(int16_t* buffer, uint32_t num_samples);

bool opensl_wrapper_init(AndroidAudioCallback cb, uint32_t frames_per_buffer_arg, uint32_t sample_rate_arg);
void opensl_wrapper_shutdown();
