
#pragma once

namespace noob
{
	class mixer
	{
		public:
			// Returns singleton object
			// This call is only allowed after a successful precedent creation of an audio device */
			static mixer& singleton()
			{
				return *s_instance;
			}

			// Calls the destructor of the singleton object
			static void destroy()
			{
				if(s_instance)
				{
					delete s_instance;
				};

				s_instance = 0;
			}

			void volume(float left, float right)
			{
				m_vol_l = left;
				m_vol_r = right;
			}

			void volume(float vol)
			{
				m_vol_l = m_vol_r = vol;
			}

			/// loads a sound sample from file, optionally adjusts volume, returns handle
			unsigned int load_sample_file(const std::string & filename, float volume = 1.0f);
			/// converts a sound sample to internal audio format, returns handle
			unsigned int load_sample_mem(const audio_sample & sample, float volume = 1.0f);
			/// deletes a previously created sound sample resource identified by its handle
			bool destroy_sample(unsigned int sample);
			/// allows read access to a sample identified by its handle
			const audio_sample* get_sample(unsigned int handle) const;


			// Disparity refers to the time difference between left and right channel in seconds. Use negative values to specify a delay for the left channel, positive for the right.
			// Pitch value of 0.5 corresponds to one octave below original sample; 2.0 to one above.
			int play_sound(unsigned int sample, float volume_left = 1.0f, float volume_right = 1.0f, float disparity = 0.0f, float pitch = 1.0f, uint32_t loop = 0);
			int loop_sample(unsigned int sample, float volume_left = 1.0f, float volume_right = 1.0f, float disparity = 0.0f, float pitch = 1.0f);
			/// Updates parameters of a currently_playing sound
			bool update_sound_params(unsigned int sound, float volume_left, float volume_right, float disparity = 0.0f, float pitch = 1.0f);
			/// Stops a specified sound immediately
			bool stop(unsigned int sound);
			/// Stops all sounds immediately
			void stop();
			/// Returns number of currently active sounds
			unsigned int num_active_sounds() const;

		protected:
			mixer();
			~mixer()
			{
				s_instance = 0;
			}

			// Stores output stream frequency
			unsigned int m_freqency_out;
			

			float m_vol_l, m_vol_r;

			/// pointer to singleton
			static mixer * s_instance;
	};
}
