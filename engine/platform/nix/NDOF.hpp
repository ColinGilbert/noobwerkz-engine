#pragma once
#include "MathFuncs.hpp"
#include "NoobUtils.hpp"

namespace noob
{
	class ndof
	{
		public:
			struct data
			{
				bool movement;
				noob::vec3 translation;
				noob::vec3 rotation;
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

			bool init();
			void run();
			noob::ndof::data get_data();
			//std::vector<size_t> get_button_presses();
			//std::vector<size_t> get_held_buttons();
		protected:
			double x, y, z, rx, ry, rz;
			//std::vector<size_t> held_buttons;
			size_t ticks;
	};
}
