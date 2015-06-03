#pragma once

#include "UI.hpp"

namespace noob
{
	class gui
	{
		public:
			void init();
			void input_key(char c);
			void resize_window(float width, float height);

		protected:
			float width, height;
	};


}
