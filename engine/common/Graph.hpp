// This class is currently a write-only graph, as removing vertices is an ugly operation that needs refreshing the structure.

#pragma once

#include <cstdint>
#include <limits>

#include <rdestl/vector.h>
#include <rdestl/hash_map.h>

namespace noob
{
	class graph
	{
		struct halfedge
		{
			halfedge() : parent(std::numeric_limits<uint32_t>::max), child(std::numeric_limits<uint32_t>::max) {}
			uint32_t parent, child;
		};

		public:
			uint32_t add_vertex()  noexcept;
			void add_half_edge(uint32_t parent, uint32_t child) noexcept;
			rde::vector<uint32_t> get_children(uint32_t vertex) const noexcept;
			rde::vector<uint32_t> get_parents(uint32_t vertex) const noexcept;
			bool is_child_of(uint32_t potential_child, uint32_t potential_parent) const noexcept;
			bool is_parent_of(uint32_t potential_parent, uint32_t potential_child) const noexcept;

		protected:
			rde::hash_map<uint32_t, rde::vector<uint32_t>> adjacencies;

	};
}
