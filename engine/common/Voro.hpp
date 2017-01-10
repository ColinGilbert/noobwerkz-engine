#pragma once

// External libs
#include <voro++.hh>


namespace noob
{
	class voro_cell
	{
		public:

		protected:

		voro::voronoicell impl;
	};

	typedef noob::handle<noob::voro_cell> voro_cell_handle;
}
