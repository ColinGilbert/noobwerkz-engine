// Currently, this map lacks the ability to lookup parents. This may change. Also, the edges are stored as a hash-map, which takes up more memory than it otherwise could. Benchmark and fix?
#pragma once

#include <cstdint>
#include <limits>
#include <assert.h>

#include <rdestl/vector.h>
#include <rdestl/rdestl.h>
#include <rdestl/hash_map.h>



namespace noob
{
	typedef uint32_t node_h;

	class dynamic_graph
	{
		public:
			static const uint32_t invalid = std::numeric_limits<uint32_t>::max();

			dynamic_graph() noexcept(true) : n_edges(0), n_nodes(1), nodes({true}) {}

			// No copying (or moving, for now...)
			dynamic_graph(const noob::dynamic_graph&) = delete;
			dynamic_graph(const noob::dynamic_graph&&) = delete;
			~dynamic_graph() noexcept(true) {}
			// No assignment (for now...)
			noob::dynamic_graph& operator=(const noob::dynamic_graph&) = delete;
			noob::dynamic_graph& operator=(const noob::dynamic_graph&&) = delete;

			noob::node_h add_node() noexcept(true)
			{
				++n_nodes;
				nodes.push_back(true);
				return nodes.size()-1;
			}

			void del_node(node_h n) 
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

			void add_edge(noob::node_h a, noob::node_h b) noexcept(true)
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
							rde::vector<node_h> v;
							v.push_back(b);
							// Insert it.
							edges.insert(rde::make_pair(a, v));
						}
						// Iterate through A's children to see if there is already an edge between A and B.
						else
						{
							bool b_found = false;
							for (node_h i: (it->second))
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
								rde::vector<node_h> v(it->second);
								v.push_back(b);
								rde::quick_sort(&v[0], &v[0]+ v.size());
								edges.erase(a);
								edges.insert(rde::make_pair(a, v));
							}
						}
					}
				}
			}

			void del_edge(noob::node_h a, noob::node_h b) noexcept(true)
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
								rde::vector<node_h> temp;
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

			rde::vector<node_h> get_children(node_h n) noexcept(true)
			{
				auto it = edges.find(n);
				if (it != edges.end())
				{
					return it->second;
				}
				rde::vector<node_h> results;
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

			// TODO: Find out a way to compress and topologically sort the nodes vector
			void garbage_collect() noexcept(true)
			{
				// First, always ensure the root node always exists. :)
				nodes[0] = true;
				for (uint32_t i = 0; i < nodes.size(); ++i)
				{
					fix_children(i);
				}
			}

			// Each pair returned symbolizes a mapping from an old position to a new one. Used to update client-side code.
			rde::vector<rde::pair<node_h, node_h>> topological_sort() noexcept(true)
			{


			}

			void fix_children(node_h n) noexcept(true)
			{
				// Our first safety/optimization: If the index is out of range, don't even do the rest.
				if (n < nodes.size())
				{
					// Another optimization: If the parent node is invalid it's safe to just delete all its links downwards.
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
							for (node_h c: it->second)
							{
								if (!nodes[c])
								{
									all_there = false;
									// 
									break;
								}
							}
							// If any are deleted, loop over the children again and collect only the live ones. Then, delete the old edges and reinsert the filtered ones.
							if (!all_there)
							{
								rde::vector<node_h> filtered;
								for (uint32_t i = 0; i < (it->second).size(); ++i)
								{
									// "c" is for "child" :)
									node_h c = (it->second)[i];

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
				}
			}


		protected:
			uint32_t n_edges, n_nodes;
			// the boolean indicates whether the node is still valid (or whether it got deleted and is awaiting garbage collection)
			rde::vector<bool> nodes;
			rde::hash_map<noob::node_h, rde::vector<noob::node_h>> edges;
	};
}
