#pragma once

#include <limits>
#include <rdestl/rdestl.h>
#include <rdestl/fixed_array.h>

namespace noob
{
	template <uint32_t N>
	class handle_map
	{
		public:
			static const uint16_t limits = std::numeric_limits<uint16_t>::max();
		
			


			uint32_t add(uint32_t k) noexcept(true)
			{
			}

			rde::pair<uint32_t, uint32_t> erase(uint32_t k) noexcept(true)
			{
				if (size > 0)
				{
					uint16_t val = keys[k];
					uint16_t last_physical = size - 1;
				}
			}

			uint32_t get_val_for_key(uint32_t k) noexcept(true)
			{
				
			}

			uint32_t get_key_for_val(uint32_t v) noexcept(true)
			{
				
			}

			void clear() noexcept(true)
			{
				
			}


		protected:
			uint32_t size;
			rde::fixed_array<uint16_t, N> keys;
			rde::fixed_array<uint16_t, N> values;
			
	};
}
