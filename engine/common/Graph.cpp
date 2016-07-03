#include "Graph.hpp"

uint32_t noob::dynamic_graph::add_node() noexcept(true)
{
	++n_nodes;
	nodes.push_back(true);
	return nodes.size() - 1;
}


void noob::dynamic_graph::del_node(uint32_t n) noexcept(true)
{
	--n_nodes;
	nodes[n] = false;
	node_dels.push_back(n);
	dirty = true;
}


void noob::dynamic_graph::add_edge(uint32_t a, uint32_t b) noexcept(true)
{
	if (a > nodes.size() - 1 || b > nodes.size() - 1) return;// false;
	if (!nodes[a] || !nodes[b]) return;// false;

	++add_to_end;
	++n_edges;
	edges.push_back(edge(a,b));
}


void noob::dynamic_graph::del_edge(uint32_t a, uint32_t b) noexcept(true)
{
	dirty = true;
	edge_dels.push_back(edge(a,b));
	--n_edges;
}


rde::vector<uint32_t> noob::dynamic_graph::get_children(uint32_t n) const noexcept(true)
{
	rde::vector<uint32_t> results;

	uint32_t last_sorted_pos = edges.size() - add_to_end;

	rde::vector<edge>::iterator it = const_cast<const rde::vector<edge>::iterator>(rde::lower_bound(&edges[0], &edges[last_sorted_pos], edge(n, 0), rde::less<edge>()));

	while (it->a == n)
	{
		if (it->b != std::numeric_limits<uint32_t>::max()) 
		{
			results.push_back(it->b);
		}
		++it;
	}

	if (dirty)
	{
		for (uint32_t i = last_sorted_pos; i < edges.size(); ++i)
		{
			edge e = edges[i];
			// if (e.a != std::numeric_limits<uint32_t>::max())
			// {
			if (e.a == n) results.push_back(e.b);
			// }
		}
	}
	return results;
}


uint32_t noob::dynamic_graph::num_nodes() const noexcept(true)
{
	return n_nodes;
}


uint32_t noob::dynamic_graph::num_edges() const noexcept(true)
{
	return n_edges;
}


bool noob::dynamic_graph::has_loops() const noexcept(true)
{
	traveller t = get_traveller();
	


	return false;
}

// NOTE: Gets rid of all edges between invalid (ie: deleted) nodes.
void noob::dynamic_graph::garbage_collect() noexcept(true)
{
	if (dirty)
	{
		// TODO: Test if it speed things up or not...
		// rde::quick_sort(node_dels.begin(), node_dels.end(), rde::less<uint32_t>())
		nodes[0] = true;

		// Ugly set of loops. :(
		for (uint32_t edge_counter = 0; edge_counter < edges.size(); ++edge_counter)
		{
			noob::dynamic_graph::edge e = edges[edge_counter]; 

			for(uint32_t edge_del_counter = 0; edge_del_counter < edge_dels.size(); ++edge_del_counter)
			{
				if (edge_dels[edge_del_counter] == e)
				{
					e.a = e.b = std::numeric_limits<uint32_t>::max();
				}
			}


			for (uint32_t node_del_counter = 0; node_del_counter < node_dels.size(); ++node_del_counter)
			{
				if (e.a == node_dels[node_del_counter] || e.b == node_dels[node_del_counter])
				{
					e.a = e.b = std::numeric_limits<uint32_t>::max();
					edges[edge_counter] = e;
				}
			}
		}


		rde::quick_sort(&edges[0], &edges[edges.size()]);//, rde::less<edge>());

		uint32_t distance_to_invalids;
		for (uint32_t distance_to_invalids = 0; distance_to_invalids < edges.size(); ++distance_to_invalids)
		{
			// No need to test the second node, as both edge members are set to uint32_t::max by invalidating functions. 
			if (edges[distance_to_invalids].a == std::numeric_limits<uint32_t>::max()) 
			{
				break;
			}
		}

		edge_dels.resize(0);

		edges.resize(distance_to_invalids);

		dirty = false;
	}
}

