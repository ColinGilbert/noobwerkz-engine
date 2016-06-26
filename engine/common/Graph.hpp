// Currently, this map lacks the ability to rapidly lookup parents. This may change. Also, the edges are stored as a hash-map, which takes up more memory than it otherwise could. Benchmark and fix?
// Nodes that are marked as invalid get ignored until discarded upon rebuild.

#include <cstdint>
#include <limits>
#include <assert.h>

#include <rdestl/vector.h>
#include <rdestl/rdestl.h>
#include <rdestl/hash_map.h>
#include <rdestl/algorithm.h>
#include "HandleMap.hpp"

namespace noob
{

	class implicit_graph
	{
		public:
			uint32_t get_next_child(uint32_t n)
			{
			}


		private:
			rde::vector<uint16_t> nodes;
	};

	class dynamic_graph
	{
		public:
			static const uint32_t invalid = std::numeric_limits<uint32_t>::max();

			struct node_h
			{
				node_h() noexcept(true) : inner(invalid) {}
				node_h(uint32_t i) noexcept(true) : inner(i) {}
				uint32_t inner;
			};

			dynamic_graph() noexcept(true) : n_edges(0), n_nodes(1), nodes({true}) {}

			//dynamic_graph(const noob::dynamic_graph&) = delete;
			//dynamic_graph(const noob::dynamic_graph&&) = delete;
			~dynamic_graph() noexcept(true) {}
			// No assignment (for now...)
			//noob::dynamic_graph& operator=(const noob::dynamic_graph&) = delete;
			//noob::dynamic_graph& operator=(const noob::dynamic_graph&&) = delete;

			uint32_t add_node() noexcept(true)
			{
				++n_nodes;
				nodes.push_back(true);
				return nodes.size()-1;
			}

			void del_node(uint32_t n) 
			{
				if (n > 0)
				{
					bool found = false;
					if (n < nodes.size())
					{
						bool valid = false;
						if (nodes[n])
						{
							valid = true;
							--n_nodes;
							nodes[n] = false;
						}

						if (valid)
						{
							garbage_collect();
						}
					}
				}
			}

			void add_edge(uint32_t a, uint32_t b) noexcept(true)
			{
				if (a < nodes.size() && b < nodes.size())
				{
					if (nodes[a] && nodes[b])
					{
						// No entry in the hashtable for A means no children. Create a vector with B in it and insert it.
						auto it = edges.find(a);
						if (it == edges.end())
						{
							// Create a vector with B.
							rde::vector<uint32_t> v;
							v.push_back(b);
							// Insert it.
							edges.insert(rde::make_pair(a, v));
						}

						// Iterate through A's children to see if there is already an edge between A and B.
						else
						{
							bool b_found = false;
							for (uint32_t i: (it->second))
							{
								if (i == b)
								{
									b_found = true;
								}
							}
							// If not, add it.
							if (!b_found)
							{
								++n_edges;
								rde::vector<uint32_t> v(it->second);
								v.push_back(b);
								rde::quick_sort(&v[0], &v[0]+ v.size());
								edges.erase(a);
								edges.insert(rde::make_pair(a, v));
							}
						}
					}
				}
			}

			void del_edge(uint32_t a, uint32_t b) noexcept(true)
			{
				if (a < nodes.size() && b < nodes.size())
				{
					if (nodes[a] && nodes[b])
					{
						// Find A's children
						auto it = edges.find(a);
						// Now, if A even has any...
						if (it != edges.end())
						{
							uint32_t found_at = invalid;

							// Find if B is on the list and if so, record where it was found.
							for (uint32_t i = 0; i < (it->second).size(); ++i)
							{
								if ((it->second)[i] == b)
								{
									found_at = i;
									break;
								}
							}

							// If we actually found node B on our list, we proceed with the rest of our method...
							if (found_at != invalid)
							{
								// We can assume that we're getting rid of at least one edge.
								--n_edges;
								// Get rid of our old list...
								rde::vector<uint32_t> temp;
								for (uint32_t i = 0; i < (it->second).size(); ++i)
								{
									if (i != found_at)
									{
										uint32_t index = (it->second)[i];
										// If our child hasn't been deleted, we keep it.
										if (nodes[index])
										{
											temp.push_back(index);
										}
										// Otherwise decrement the edge count.
										else
										{
											--n_edges;
										}
									}
								}

								// Replace the edges.
								edges.erase(a);
								edges.insert(rde::make_pair(a, temp));
							}
						}
					}
				}
			}

			rde::vector<uint32_t> get_children(uint32_t n) const noexcept(true)
			{
				auto it = edges.find(n);
				if (it != edges.end())
				{
					return it->second;
				}
				rde::vector<uint32_t> results;
				return results;
			}

			uint32_t num_nodes() const noexcept(true)
			{
				return n_nodes;
			}

			uint32_t num_edges() const noexcept(true)
			{
				return n_edges;
			}


			// TODO: Multithread.
			bool has_loops() const noexcept(true)
			{
				bool found, exhausted;
				found = exhausted = false;

				traveller t = get_traveller();
				t.teleport(0);

				return false;
			}

			// NOTE: Gets rid of all edges between invalid nodes.
			void garbage_collect() noexcept(true)
			{	
				// First, always ensure the root node always exists. :)
				nodes[0] = true;

				rde::vector<bool> visited(nodes.size());
				rde::fill_n<bool>(&visited[0], visited.size(), false);

				for (uint32_t i = 0; i < nodes.size(); ++i)
				{
					rde::vector<uint32_t> children = fix_children_with_retval(i);
					for (uint32_t c : children)
					{
						visited[c] = true;
					}
				}
			}

			// The bool refers to whether the sort succeeded or not, and each pair symbolizes a mapping from an old position to a new one. These results are used to update client-side code.
			rde::pair<bool, rde::vector<rde::pair<uint32_t, uint32_t>>> topological_sort() noexcept(true)
			{

			}

			class traveller
			{
				friend class dynamic_graph;
				public:
				traveller() noexcept(true) : lookat(0), depth(0) {}

				void teleport(uint32_t n) noexcept(true)
				{
					path.clear();
					lookat = 0;
					depth = 0;
				}

				uint32_t get_current() const noexcept(true)
				{
					return path[depth];
				}

				uint32_t get_lookat_node() const noexcept(true)
				{
					return (it_ref->second)[lookat];
				}

				uint32_t get_lookat_index() const noexcept(true)
				{
					return lookat;
				}

				uint32_t get_depth() const noexcept(true)
				{
					return depth;
				}

				bool go_up() noexcept(true)
				{
					// Nowhere to go upward.
					if (depth == 0) return false;
					// Parent invalidated since last visit.
					if (!nodes_ref[path[depth-1]]) return false;

					--depth;
					it_ref = map_ref.find(nodes_ref[path[depth]]);
					lookat = 0;
					return true;
				}

				bool go_down() noexcept(true)
				{
					uint32_t target = (it_ref->second)[lookat];
					if (!nodes_ref[target]) return false;
					it_ref = map_ref.find(target);
					lookat = 0;
					++depth;
					path.reserve(depth);
					path[depth] = target;
				}

				bool go_left() noexcept(true)
				{
					if (lookat == 0) return false;

					--lookat;
					return true;
				}

				bool go_right() noexcept(true)
				{
					if (lookat >= (it_ref->second).size()) return false;

					++lookat;
					return true;
				}



				protected:
				uint32_t lookat, depth;
				rde::hash_map<uint32_t, rde::vector<uint32_t>>& map_ref;
				rde::vector<bool>& nodes_ref;
				rde::hash_map<uint32_t, rde::vector<uint32_t>>::iterator& it_ref; 
				rde::vector<uint32_t> path;
			};



			traveller get_traveller() const
			{
				traveller results;

				results.map_ref = edges;
				results.nodes_ref = nodes;
				results.it_ref = edges.end();
				return results;
			}



		protected:

			// Both the methods below require you to pass in a number < nodes.size(). This is why they're marked as protected
			// This simply calls the fix_children_with_ret function and does nothing with the results.
			void fix_children(uint32_t n) noexcept(true)
			{
				auto a = fix_children_with_retval(n);
			}

			// Gives you back the list of valid children.
			rde::vector<uint32_t> fix_children_with_retval(uint32_t n) noexcept(true)
			{
				// The vector of filtered children that are safe to travel.
				rde::vector<uint32_t> filtered;

				// An optimization: If the parent node is invalid it's safe to just delete all its links downwards. :)
				if (!nodes[n])
				{
					auto it = edges.find(n);
					if (it != edges.end())
					{
						n_edges -= (it->second).size();
						edges.erase(n);
					}
				}
				else
				{
					auto it = edges.find(n);
					// If this node even has any...
					if (it != edges.end())
					{
						// Loop over the children and see if any are deleted or not.
						bool all_there = true;
						for (uint32_t c: it->second)
						{
							if (!nodes[c])
							{
								all_there = false;
								break;
							}
						}
						// If any are deleted, loop over the children again and collect only the live ones. Then, delete the (now invalid) edges and reinsert the filtered ones.
						if (!all_there)
						{
							for (uint32_t i = 0; i < (it->second).size(); ++i)
							{
								// "c" is for "child" :)
								uint32_t c = (it->second)[i];

								if (nodes[c])
								{
									filtered.push_back(c);
								}
								else
								{
									--n_edges;
								}
							}
							edges.erase(n);
							edges.insert(rde::make_pair(n, filtered));
						}
					}
				}

				return filtered;
			}

			// This is given a path with the candidate at the end and returns the first index that loops with it. The search starts at index "from".
			// TODO: This is a rather general numerical function, and should be in a better-planned header
			// NOTE: Will likely crash if "from" isn't lower than path.size() and if "path" is smaller than 1. :P This is why this function is (temporarily) in a protected namespace.
			uint32_t find_first_loop(const rde::vector<uint32_t>& path, uint32_t from)
			{
				const uint32_t candidate_pos = path.size() - 1;
				// The candidate is at the end...
				uint32_t candidate = path[candidate_pos];
				bool found = false;
				// If the candidate node is even valid...
				if (nodes[candidate])
				{
					for (uint32_t i = from; i < candidate_pos; ++i)
					{
						if (path[i] == candidate)
						{
							return i;
						}
					}
				}

				return invalid;
			}

			// Data members
		protected:

			uint32_t n_edges, n_nodes;
			// The boolean indicates whether the node is still valid (or whether it got deleted and is awaiting garbage collection)
			rde::vector<bool> nodes;
			rde::hash_map<uint32_t, rde::vector<uint32_t>> edges;
	};
}
