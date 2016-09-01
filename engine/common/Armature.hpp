#pragma once

#include <tuple>

#include <rdestl/fixed_array.h>
#include <rdestl/vector.h>

#include "NoobDefines.hpp"
#include "Component.hpp"
#include "ComponentDefines.hpp"
#include "NoobCommon.hpp"
#include "ScaledModel.hpp"
#include "ShadingVariant.hpp"

namespace noob
{
	class armature
	{
		public:
			// armature() : models(graph), /*surfaces(graph),*/ bind_pose(graph) {}

		protected:
			// lemon::StaticDigraph graph;

			//lemon::StaticDigraph::NodeMap<noob::scaled_model> models;
			// lemon::StaticDigraph::NodeMap<std::tuple<noob::vec3, noob::versor>> bind_pose;
	};

	typedef noob::component_dynamic<noob::armature> armatures_holder;
	typedef noob::handle<noob::armature> armature_handle;


	// These are used to evaluate the effect of various forces. For use within a scene.
	class boss_armature
	{
		public:
			// boss_armature() : bodies(graph), /*surfaces(graph),*/ joints(graph) {}

		protected:
			// lemon::StaticDigraph graph;
			// lemon::StaticDigraph::NodeMap<noob::body_handle> bodies;
			// lemon::StaticDigraph::NodeMap<noob::surface> surfaces;
			// lemon::StaticDigraph::ArcMap<noob::joint_handle> joints;
	};

	typedef noob::component_dynamic<noob::boss_armature> boss_armatures_holder;
	typedef noob::handle<noob::boss_armature> boss_armature_handle;


	// These are used to dynamically create new armatures. For use within a scene.
	class dynamic_armature
	{
		public:	
			// dynamic_armature() : bodies(graph), /*surfaces(graph),*/ joints(graph) {}

		protected:
			// lemon::ListDigraph graph;
			// lemon::ListDigraph::NodeMap<noob::body_handle> bodies;
			// lemon::ListDigraph::NodeMap<noob::surface> surfaces;
			// lemon::ListDigraph::ArcMap<noob::joint_handle> joints;
	};

	typedef noob::component_dynamic<noob::dynamic_armature> dynamic_armatures_holder;
	typedef noob::handle<noob::dynamic_armature> dynamic_armature_handle;

	// This stores an entire set of bone positions + orientations. Meant to be used as a reliable API for info exchange. Associated with a skeleton.
	struct armature_keyframe
	{
		// armature_keyframe(lemon::StaticDigraph& g) : keyframes(g) {}

		float t;
		//lemon::StaticDigraph::NodeMap<std::tuple<noob::vec3, noob::versor>> keyframes;
	};
}
