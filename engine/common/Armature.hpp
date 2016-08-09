#pragma once

#include <tuple>

#include <lemon/static_graph.h>
#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "Globals.hpp"
#include "NoobDefines.hpp"
#include "ComponentDefines.hpp"
#include "NoobCommon.hpp"


namespace noob
{
	typedef rde::fixed_array<noob::drawing_info, 32> character_colour_scheme;
	typedef rde::vector<noob::drawing_info> boss_colour_scheme;

	class armature
	{
		public:
			armature() : models(graph), bind_pose(graph) {}

		protected:
			lemon::StaticDigraph graph;
			lemon::StaticDigraph::NodeMap<noob::model_handle> models;
			lemon::StaticDigraph::NodeMap<std::tuple<noob::vec3, noob::versor>> bind_pose;
	};

	struct armature_keyframe
	{
		armature_keyframe(lemon::StaticDigraph& g) : keyframes(g) {}
		
		float t;
		lemon::StaticDigraph::NodeMap<std::tuple<noob::vec3, noob::versor>> keyframes;
	};

	// These are used to evaluate the effect of various forces. For use within a scene.
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

	// These are used to dynamically create new armatures. For use within a scene.
	class dynamic_armature
	{
		public:	
			dynamic_armature() : bodies(graph), shadings(graph), joints(graph) {}

		protected:
			lemon::ListDigraph graph;
			lemon::ListDigraph::NodeMap<noob::body_handle> bodies;
			lemon::ListDigraph::NodeMap<noob::drawing_info> shadings;
			lemon::ListDigraph::ArcMap<noob::joint_handle> joints;
	};
}
