#pragma once

namespace noob
{
	class physics
	{
		public:
			typedef noob::handle<uint32_t> shape_handle;
			typedef noob::handle<uint32_t> body_handle;
			typedef noob::handle<uint32_t> ghost_handle;

		protected:
			std::vector<noob::shape_handle> body_shapes;
			std::vector<noob::shape_handle> ghost_shapes;
	};
}
