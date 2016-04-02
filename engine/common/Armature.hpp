// In this implementation, an armature is a graph consisting of the body tags (nodes) and joint tags (edges.)
// This structure is then passed to a scene and used to snapshot animation sequences.

#pragma once

#include "SkeletalAnim.hpp"
#include "Joint.hpp"
#include <lemon/smart_graph.h>


namespace noob
{
	class armature
	{
		public:
			armature() : bodies_mapping(graph), joint_mapping(graph) {}
			// void init(noob::animated_model* model_ptr, noob::skeletal_anim* anim)
			// void add ();

		protected:
			lemon::SmartDigraph graph;
			lemon::SmartDigraph::NodeMap<size_t> bodies_mapping;
			lemon::SmartDigraph::ArcMap<size_t> joint_mapping;
	};
}
