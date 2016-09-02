#include "Graph.hpp"
#include "Logger.hpp"
#include <cassert>
#include <iostream>
#include <sstream>

int main()
{
	noob::graph g;
	
	const noob::node_handle first = g.add_node();
	const noob::node_handle second = g.add_node();

	assert(!g.path_exists(first, second) && "First & second - Path should not exist!");

	g.add_path(first, second);

	assert(g.path_exists(first, second) && "First & second - Path should exist!");

	const noob::node_handle third = g.add_node();
	const noob::node_handle fourth = g.add_node();
	const noob::node_handle fifth = g.add_node();

	g.add_path(first, third);
	
	assert(g.path_exists(first, third) && "First and third - Path should exist!");

	g.add_path(second, fourth);

	assert(g.path_exists(second, fourth) && "Second and fourth - Path should exist!");

	rde::vector<noob::node_handle> children = g.get_children(second);
	std::stringstream ss;

	ss << "Children of node 2:";
	for (noob::node_handle h : children)
	{
		ss << " " << h.index();
	}

	noob::logger::log(ss.str());
	assert(!g.path_exists(second, third) && "Second and third - Path should not exist!");


	// Up to this point, the graph has been acyclic. Further on, we test iterators and loops!
	
	


	std::cout << "ALL PASS!\n";


	return 0;
}
