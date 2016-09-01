#include "Graph.hpp"

#include <cassert>
#include <iostream>

int main()
{
	noob::graph g;
	
	const uint32_t first = g.add_node();
	const uint32_t second = g.add_node();

	assert(!g.path_exists(first, second) && "First & second - Path should not exist!");

	g.add_path(first, second);

	assert(g.path_exists(first, second) && "First & second - Path should exist!");

	const uint32_t third = g.add_node();
	const uint32_t fourth = g.add_node();
	const uint32_t fifth = g.add_node();

	g.add_path(first, third);
	
	assert(g.path_exists(first, third) && "First and third - Path should exist!");

	g.add_path(second, fourth);

	assert(g.path_exists(second, fourth) && "Second and fourth - Path should exist!");

	assert(!g.path_exists(second, third) && "Second and third - Path should not exist!");

	// Up to this point, the graph has been acyclic. Further on, we test iterators and loops!




	std::cout << "ALL PASS!\n";


	return 0;
}
