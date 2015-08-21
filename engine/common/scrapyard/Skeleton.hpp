#pragma once

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>
#include "MathFuncs.hpp"

namespace noob
{
	class hierarchical_transforms
	{
		public:
			bool valid() const;
			size_t get_node_id(const std::string& name) const;
			std::string get_node_name(size_t) const;
			size_t get_num_nodes() const;

			// The following two methods return info in model-space, in order to quickly draw.
			noob::mat4 get_node_transform(size_t) const;
			std::vector<noob::mat4> get_transforms() const;

			// relative_to_parent refers to the transform being in relation to the parent node, as opposed to being in model-space. Root node is always relative to origin. Always. Mathematical info refers to the bind pose of the skeleton.
			void add_node(const std::string& name, const std::string& parent, const noob::versor& rotation, const noob::vec3& translation, const noob::vec3& scale, bool relative_to_parent = true);
			// This turns our skeleton from a linked-list implementation to a static array implementation for (far) greater speed.
			void finalize_graph();

			// Both of these should be called after finalize_graph()
			void add_animation(const std::string& anim_name, size_t num_keyframes);
			void set_node(const std::string& anim_name, size_t keyframe, const std::string& node_name, const noob::versor& rotation, const noob::vec3& translation, const noob::vec3& scale, bool relative_to_parent = true);



		protected:
			lemon::ListDigraph construction_graph;
			lemon::StaticDigraph iterable_graph;
	};
}
