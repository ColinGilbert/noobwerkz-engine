#pragma once

#include <cstdint>
#include <limits>
#include <assert.h>

#include <rdestl/vector.h>
#include <rdestl/slist.h>


namespace noob
{
	struct graph
	{

		struct node
		{
			node() noexcept(true) : valid(true) {}
			bool valid;
			rde::vector<uint32_t> outgoing;

		};

		// Member functions
		bool node_exists(uint32_t n) const
		{
			if (n > nodes.size() - 1)
			{
				return false;
			}
			return nodes[n].valid;
		}

		rde::vector<node> nodes;
	};
}
