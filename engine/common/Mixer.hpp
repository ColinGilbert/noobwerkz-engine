#pragma once

#include <rdestl/vector.h>
#include <rdestl/pair.h>
#include "Component.hpp"
#include "ComponentDefines.hpp"

namespace noob
{
	class sound_pool
	{
		
		public:
			// First element is start index. Last element is stop index.
			uint32_t add_clip(const rde::vector<uint16_t>& samples, const rde::string& name)
			{
				
			}

		protected:
			rde::vector<uint16_t> pool;
			noob::component<uint32_t> start_positions;
	};
}
