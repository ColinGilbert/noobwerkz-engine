#pragma once

#include <tuple>

#include <lemon/static_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>

#include <rdestl/vector.h>

#include "Globals.hpp"
#include "NoobDefines.hpp"
#include "ComponentDefines.hpp"
#include "NoobCommon.hpp"


namespace noob
{
	class armature
	{
		public:
			armature() : shapes(graph), bind_pose(graph) {}

		protected:
			lemon::StaticDigraph graph;
			lemon::StaticDigraph::NodeMap<noob::shape_handle> shapes;
			lemon::StaticDigraph::NodeMap<std::tuple<noob::vec3, noob::versor>> bind_pose;
	};

	// These get used from inside a stage. You want to create them from armature_builder types.
	class active_armature
	{
		public:
			active_armature() : bodies(graph), shadings(graph), joints(graph) {}

		protected:
			lemon::StaticDigraph graph;
			lemon::StaticDigraph::NodeMap<noob::body_handle> bodies;
			lemon::StaticDigraph::NodeMap<noob::drawing_info> shadings;
			lemon::StaticDigraph::ArcMap<noob::joint_handle> joints;
	};

	// These get used from inside a stage. Armatures are created from these, and then added to a global scope for reuse.
	class armature_builder
	{
		public:	
		armature_builder() : bodies(graph), shadings(graph), joints(graph) {}

		// noob::armature get_armature() const;

		protected:
			lemon::ListDigraph graph;
			lemon::ListDigraph::NodeMap<noob::body_handle> bodies;
			lemon::ListDigraph::NodeMap<noob::drawing_info> shadings;
			lemon::ListDigraph::ArcMap<noob::joint_handle> joints;
	};
}
