// Used to represent input from SpaceMouse NDOF, which was dead-simple to use along with libspnav (linked-in separately
#pragma once

#include <cstddef>

namespace noob
{
	class ndof
	{
		public:
			struct data
			{
				bool valid;
				float x, y, z, rx, ry, rz;
			};

			enum button_state
			{
				PRESS, RELEASE, HELD
			};

			struct button_press
			{
				size_t button;
				noob::ndof::button_state state;
			};

			// bool init();
			void run();
			noob::ndof::data get_data();
			bool has_data() const;
			//std::vector<size_t> get_button_presses();
			//std::vector<size_t> get_held_buttons();
		protected:
			int x, y, z, rx, ry, rz;
			
			//std::vector<size_t> held_buttons;
			size_t ticks;
	};
}
