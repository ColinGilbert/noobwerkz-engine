#pragma once

#include <rdestl/vector.h>
#include <limits>

namespace noob
{
	class handle_map
	{
		public:
			handle_map() noexcept(true) {}
			
			static const uint32_t invalid = std::numeric_limits<uint32_t>::max();
			
			void add_pair(uint32_t k, uint32_t v) noexcept(true)
			{

			}
			
			void del_pair(uint32_t k, uint32_t v) noexcept(true)
			{

			}

			void add_pairs(const rde::vector<rde::pair<uint32_t, uint32_t>>& p) noexcept(true)
			{
			
			}

			void del_pairs(const rde::vector<rde::pair<uint32_t, uint32_t>>& p) noexcept(true)
			{

			}

			rde::vector<uint32_t> get_values_for_key(uint32_t k) noexcept(true)
			{

			}

			rde::vector<uint32_t> get_keys_for_value(uint32_t v) noexcept(true)
			{
				
			}


		protected:
			rde::vector<uint32_t> keys;
			rde::vector<uint32_t> values;
			
	};
}
