// Class representing an audio sample. Note: It isn't proper C++ style, but we'll overlook it for now...
#pragma once

#include <limits>

namespace noob
{
	class audio_sample
	{
		public:
			//constructor from memory data
			audio_sample(uint8_t * data, uint32_t size, uint16_t channels, uint32_t sampleRate, uint16_t bitsPerSample) : m_data(data), m_size(size), m_channels(channels), m_sampleRate(sampleRate), m_bits_per_sample(bitsPerSample) { }
			// copy constructor
			audio_sample(const audio_sample & source);

			// destructor
			~audio_sample() { delete[] m_data; }

			/// allows accessing sample data
			uint8_t * data() { return m_data; };
			/// allows reading sample data
			const uint8_t * data() const { return m_data; };


			/// Returns sample size in bytes
			uint32_t size() const { return m_size; }
			/// Returns sample size in number of frames
			uint32_t num_frames() const { return m_size / m_channels / (m_bits_per_sample >> 3); }
			/// Returns size of a single frame in bytes
			uint32_t frame_size() const { return m_channels * (m_bits_per_sample >> 3); }
			/// Returns number of parallel channels, 1 mono, 2 stereo
			uint16_t num_channels() const { return m_channels; }
			/// Returns number of frames per second, e.g., 44100, 22050
			uint32_t sample_rate() const { return m_sampleRate; }
			/// Returns number of bits per mono sample, e.g., 8, 16
			uint16_t bits_per_sample() const { return m_bits_per_sample; }
			/// Returns number of bytes per sample, e.g., 1, 2
			uint16_t bytes_per_sample() const { return m_bits_per_sample >> 3; }
			// Converts to a different bit rate, e.g., 8, 16
			bool rate_convert(uint16_t bits)
			{
				if (bits == 16)
				{
					if (m_bits_per_sample == 8)
					{
						uint8_t* data = new uint8_t[2 * m_size];
						for(size_t i = 0; i < m_size; ++i)
						{
							int16_t* ptr = static_cast<int16_t*>(data+i);
							*ptr = m_data[i] * 255;
						}
						delete [] m_data;
						m_data = data;
						m_size *= 2;
						return true;
					}
					else if(m_bits_per_sample == 16)
					{
						return true; // Nothing to do
					}
					else if(m_bits_per_sample == 32) // Float, normalized from -1.0f to 1.0f
					{
						uint8_t* data = new uint8_t [m_size / 2];
						for(size_t i = 0; i < m_size / 4; ++i)
						{
							int16_t* ptr = static_cast<int16_t*>(data + i);
							float* src= static_cast<float*>(m_data + i);
							*ptr = static_cast<int16_t>((*src) * std::numeric_limits<int16_t>::max());
						}
						delete [] m_data;
						m_data = data;
						m_size /= 2;
						return true;
					}
				}
				{
					fmt::MemoryWriter ww;
					ww << "[AudioSample] ERROR: Conversion from " << m_bits_per_sample << " to " << bits " << not supported!";
					logger::log(ww.str());
				}
				return false;
			}


			// Changes volume by given factor
			void set_volume(float f);

			/// loads a WAV file
			static audio_sample* loadWav(const std::string & fname);
			/// reads WAV data from a stream via a function compatible to std::fread
			static audio_sample* readWav(FILE* stream, size_t (*readFunc)( void *, size_t, size_t, FILE *));
		protected:
			/// stores sample data
			uint8_t * m_data;
			/// sample size in bytes
			uint32_t m_size;
			/// number of parallel channels, 1 mono, 2 stereo
			uint16_t m_channels;
			/// number of samples per second, e.g., 44100, 22050
			uint32_t m_sampleRate;
			/// number of bits per sample, e.g., 8, 16
			uint16_t m_bits_per_sample;
	};
}
