#include <noob/graph/graph.hpp>
#include <noob/logger/logger.hpp>

#include <noob/strings/strings.hpp>

#include <cassert>
#include <iostream>

// #include <sstream>

namespace noob
{
	struct log_funct
	{
		static void log(noob::importance imp, const std::string& s)
		{
			std::cout << noob::concat(noob::to_string(imp), ": ", s, "\n");
		}
	};

	typedef logger_impl<log_funct> logger;
}



int main()
{
	// Test logging.

	noob::logger::log(noob::importance::INFO, noob::concat("Testing logger! ", noob::to_string(0.9998)));

	// Test graph:
	noob::digraph g;

	assert(g.is_ready() == false);

	assert(g.num_nodes() == 0);

	const noob::node_handle a = g.add_node();
	const noob::node_handle b = g.add_node();
	const noob::node_handle c = g.add_node();

	assert(g.num_nodes() == 3);

	// Test in-order insertions
	g.add_edge(a, b);

	assert(g.edge_exists(a, b) == true);
	assert(g.edge_exists(a, c) == false);
	assert(g.edge_exists(b, a) == false);
	assert(g.edge_exists(c, a) == false);

	g.add_edge(a, c);

	assert(g.edge_exists(a, b) == true);
	assert(g.edge_exists(a, c) == true);
	assert(g.edge_exists(b, a) == false);
	assert(g.edge_exists(c, a) == false);

	assert(g.num_children(a) == 2);
	assert(g.num_children(b) == 0);
	assert(g.num_children(c) == 0);

	assert(g.is_ready() == false);

	g.sort();

	assert(g.is_ready() == true);

	rde::vector<noob::node_handle> edges_of_a;

	noob::digraph::visitor v1 = g.get_visitor(a);
	// assert(v1.is_valid() == true);
	for (uint32_t i = 0; i < v1.num_children(); ++i)
	{
		edges_of_a.push_back(v1.get_child());
	}

	std::string msg = "Children of A:";

	for (auto temp : edges_of_a)
	{
		msg = noob::concat(msg, " ", noob::to_string(temp.index()));
	}

	msg = noob::concat(msg, ". Total = ", noob::to_string(v1.num_children()));

	noob::logger::log(noob::importance::INFO, msg);

	// Now, we test with deliberately out-of-order edge insertions.


	noob::logger::log(noob::importance::INFO, noob::concat("Sorted graph - ", g.to_string()));

	const noob::node_handle d = g.add_node();
	const noob::node_handle e = g.add_node();
	const noob::node_handle f = g.add_node();

	g.add_edge(c, d);
	g.add_edge(a, f);

	assert(g.is_ready() == false);

	// We must sort the graph this time, because we inserted edges out-of-order.

	noob::logger::log(noob::importance::INFO, noob::concat("Unsorted graph - ", g.to_string()));

	g.sort();

	assert(g.is_ready() == true);

	// We must sort the graph this time, because we inserted edges out-of-order.

	noob::logger::log(noob::importance::INFO, noob::concat("Sorted graph - ", g.to_string()));

	edges_of_a.reset();
	noob::digraph::visitor v2 = g.get_visitor(a);

	while(v2.has_child())
	{
		edges_of_a.push_back(v2.get_child());
	}

	msg = "Children of A:";
	for (auto a : edges_of_a)
	{
		msg = noob::concat(msg, " ", noob::to_string(a.index()));
	}
	msg = noob::concat(msg, ". Total = ", noob::to_string(v2.num_children()));

	noob::logger::log(noob::importance::INFO, msg);


	noob::logger::log(noob::importance::INFO, "All pass! :)");

	return 0;
}
