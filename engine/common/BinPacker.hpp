// Scanline-order insertion (left-to-right, top-to-bottom) packing with normalization into floating-point

#pragma once

#include <noob/math/math_funcs.hpp>

namespace noob
{
	class bin_packer
	{
		public:
			bin_packer() noexcept(true) : bin_dims(noob::vec2ui(0, 0)), current_pos(noob::vec2ui(0,0)), space_left(false) {}
			
			struct indexed_element
			{
				
			};

			void init(const noob::vec2ui Dims);
		protected:
			struct mapped_quad
			{
				mapped_quad() noexcept(true) = default;
				mapped_quad(const noob::vec2ui TopLeftBottomRight) : topleft_bottomright(TopLeftBottomRight) {}
				noob::vec2ui topleft_bottomright;
			};

			std::vector<mapped_quad> mapped;
			std::vector<uint32_t> indexes;
			noob::vec2ui bin_dims, current_pos;
			bool space_left;
	};
}
