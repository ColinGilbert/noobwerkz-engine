// In this implementation, an armature is a graph consisting of the body tags (nodes) and joint tags (edges.)
// This structure is then passed to a scene and used to snapshot animation sequences.

#pragma once

#include "SkeletalAnim.hpp"
#include "Joint.hpp"
#include "ComponentDefines.hpp"
#include <lemon/smart_graph.h>

// #include <google/dense_hash_map>

namespace noob
{
	class armature
	{
		public:
			static const uint32_t default_nodes = 64;
			static const uint32_t default_arcs = 96;

			armature() : bodies_mapping(graph), shaders_mapping(graph), joint_mapping(graph) {}

			void init(noob::body_handle root_node);

			void append(noob::body_handle a, noob::body_handle b, noob::joint_handle j);

		protected:
			lemon::SmartDigraph::Node root;
			lemon::SmartDigraph graph;
			lemon::SmartDigraph::NodeMap<size_t> bodies_mapping;
			lemon::SmartDigraph::NodeMap<size_t> shaders_mapping;
			lemon::SmartDigraph::ArcMap<size_t> joint_mapping;

			// std::dense_hash_map<size_t, lemon::SmartDigraph::Node> bodies_to_nodes;

	};
}
