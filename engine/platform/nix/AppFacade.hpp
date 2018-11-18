// This is required due to #define conflicts between Eigen and X11... The value Success is used for both libs and life then becomes quite difficult.

#pragma once

#include <string>

#include "NDOF.hpp"

namespace noob
{
	class app_facade
	{
		public:
			void init(uint32_t Width, uint32_t Height, double DpiX, double DpiY, const std::string& FilePath);
			void window_resize(uint32_t Width, uint32_t Height);
			void step();
			void accept_ndof_data(const noob::ndof::data&);

	};
}
