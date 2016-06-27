// This class is supposed to be used internally, as a protected member of another class. This is because the get_sample() method doesn't do any bounds checking and may crash your app.

#pragma once

#include <rdestl/algorithm.h>
#include <rdestl/string.h>
#include <rdestl/vector.h>
#include <rdestl/hash_map.h>

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
				uint32_t start = starting.size();
				pool.reserve(start + samples.size());
				rde::fill_n<uint16_t>(&samples[start], samples.size();
				{
					pool.push_back(s);
				}
			}
			
			struct soundclip_results
			{	
				soundclip_results() : valid(false), start(0), end(0) {}
				bool valid;
				uint32_t start, end;
			};

			// Caution: No bounds checking.
			uint16_t get_sample(uint32_t index) const
			{
				return pool[index];
			}

		protected:
			rde::vector<uint16_t> pool;
			index_pair_holder clips;
			rde::hash_map<rde::string, rde::pair<uint32_t, uint32_t>> names;
	};
}
