#pragma once 

#include <cstdint>
#include <limits>
#include <assert.h>

#include <rdestl/vector.h>
#include <rdestl/pair.h>
#include <rdestl/algorithm.h>
#include <rdestl/functional.h>
#include <rdestl/sort.h>

namespace noob
{
	class dynamic_graph
	{
		public:
			dynamic_graph() noexcept(true) : n_nodes(1), n_edges(0), nodes({true}) {}

			uint32_t add_node() noexcept(true);

			void del_node(uint32_t n) noexcept(true);

			void add_edge(uint32_t a, uint32_t b) noexcept(true);

			void del_edge(uint32_t a, uint32_t b) noexcept(true);

			rde::vector<uint32_t> get_children(uint32_t n) const noexcept(true);

			uint32_t num_nodes() const noexcept(true);

			uint32_t num_edges() const noexcept(true);

			bool has_loops() const noexcept(true);

			// NOTE: Gets rid of all edges between invalid nodes.
			// TODO: Make it get rid of invalidated nodes too.
			void garbage_collect() noexcept(true);
			
			// The bool refers to whether the sort succeeded or not, and each pair symbolizes a mapping from an old position to a new one. These results are used to update client-side code.
			// rde::pair<bool, noob::implicit_graph> topological_sort() noexcept(true);

			// IMPORTANT: Graph travellers are a very thin abstraction intended for transient scopes and under NO circumstances should they EVER be kept around after your function is done using them.
			// They wrap around references to the hashmap and its current iterator (to avoid searching too often) and a stack of visited node id's, current one last in place.
			// This makes them very much a "read-only" tool: You cannot change the topology of an associated graph by using the tools presented in the public API of this class.
			class traveller
			{
				friend class dynamic_graph;
				
				public:
				
				void visit(uint32_t n) noexcept(true)
				{
					path.clear();
					path.push_back(n);
					lookat = current_depth = max_depth = 0;
				}

				uint32_t get_current() const noexcept(true)
				{
					return path[current_depth];
				}
				
				// TODO:
				bool has_next() const noexcept(true)
				{
					
				}

				// TODO:
				uint32_t go_next() noexcept(true)
				{

				}

				uint32_t get_lookat() const noexcept(true)
				{
					return path[lookat];
				}

				uint32_t get_depth() const noexcept(true)
				{
					return current_depth;
				}

				bool can_go_up() const noexcept(true)
				{
					if (current_depth == 0) return false;
					return true;
				}

				bool go_up() noexcept(true)
				{
					if (!can_go_up()) return false;
					--current_depth;
					children.clear();
					children = ref.get_children(path[current_depth]);
					lookat = 0;
					return true;
				}

				bool can_go_down() const noexcept(true)
				{	
					if (children[lookat]) return false;
					return false;
				}

				bool go_down() noexcept(true)
				{
					if (!can_go_down()) return false;
					uint32_t down_node = children[lookat];
					children.clear();
					children = ref.get_children(down_node);
					path.push_back(down_node);
					lookat = 0;
					++current_depth;
					max_depth = rde::max(max_depth, current_depth);	
					return true;
				}

				bool can_look_left() const noexcept(true)
				{
					if (lookat == 0) return false;
				}

				bool look_left() noexcept(true)
				{
					if (!can_look_left()) return false;
					--lookat;
					return true;
				}

				bool can_look_right() const noexcept(true)
				{
					if (lookat < children.size() -1) return true;
					return false;
				}

				bool look_right() noexcept(true)
				{
					if (!can_look_right()) return false;
					++lookat;
					return true;
				}

				const rde::vector<uint32_t>& get_path() const noexcept(true)
				{
					return path;
				}

				bool is_in_path(uint32_t n) const noexcept(true)
				{
					for (uint32_t p : path)
					{
						if (p == n) return true;
					}
					return false;
				}

				protected:
				traveller(const noob::dynamic_graph& g) noexcept(true) : ref(g), lookat(0), current_depth(0), max_depth(0), path({0}) 
				{
					children = ref.get_children(0);
				}

				static constexpr uint32_t invalid = std::numeric_limits<uint32_t>::max();
			
				const noob::dynamic_graph& ref;
				uint32_t lookat, current_depth, max_depth;
				rde::vector<uint32_t> path;
				rde::vector<uint32_t> children;
			};

			traveller get_traveller() const noexcept(true)
			{
				traveller results(*this);
				return results;
			}

		protected:

			struct edge
			{
				edge() noexcept(true) : a(std::numeric_limits<uint32_t>::max()), b(std::numeric_limits<uint32_t>::max()) {}
				
				edge(uint32_t _a, uint32_t _b) noexcept(true) : a(_a), b(_b) {}

				bool operator<(const edge& rhs) const noexcept(true)
				{
					if (a < rhs.a) return true;
					if (a == rhs.a && b < rhs.b) return true;
					return false;
				}
				bool operator==(const edge& rhs) const noexcept(true)
				{
					return (a == rhs.a && b == rhs.b);
				}
				uint32_t a, b;
			};


			// Both the methods below require you to pass in a number < nodes.size(). This is why they're marked as protected
			// This simply calls the fix_children_with_ret function and does nothing with the results.
			void fix_children(uint32_t n) noexcept(true)
			{
				auto a = fix_children_with_retval(n);
			}

			// Gives you back the list of valid children. This function is useful because you don't need to search again if you need to do stuff with the returned values.
			rde::vector<uint32_t> fix_children_with_retval(uint32_t n) noexcept(true);


		protected:

			bool dirty;
			// n_nodes and n_edges are basically used to keep the node and edge count operations down to O(1) and aren't really used for anything else.
			uint32_t n_nodes, n_edges, add_to_end;
			// The uint32_t refers to the number of children each node has.
			rde::vector<bool> nodes;
			rde::vector<edge> edges;
			rde::vector<edge> edge_dels;
			rde::vector<uint32_t> node_dels;

	};
}
