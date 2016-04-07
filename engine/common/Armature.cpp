#include "Armature.hpp"


void noob::armature::init(const noob::body_handle b)
{
	graph.reserveNode(default_nodes);
	graph.reserveArc(default_arcs);
	root = graph.addNode();
	bodies_mapping[root] = b.get_inner();
}


void noob::armature::append(noob::body_handle a, noob::body_handle b, noob::joint_handle j)
{
}
