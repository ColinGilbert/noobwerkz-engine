#pragma once

#include <lemon/static_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>

#include <rdestl/vector.h>

#include "Globals.hpp"
#include "Actor.hpp"
#include "NoobDefines.hpp"
#include "ComponentDefines.hpp"

namespace noob
{
	struct armature
	{
		public:
			armature() : nodes(graph), arcs(graph) {}

		protected:
			lemon::StaticDigraph graph;
			lemon::StaticDigraph::NodeMap<noob::basic_actor> nodes;
			lemon::StaticDigraph::ArcMap<noob::joint_handle> arcs;
	};

}
