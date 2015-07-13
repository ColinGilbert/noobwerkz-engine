// This class uses two graphs: One for modifying and another for super-fast iterating.
#pragma once

#include <PhysicsWorld.hpp>
#include <Graphics.hpp>
#include <MathFuncs.hpp>
#include <VoxelWorld.hpp>
#include <HACDRenderer.hpp>
#include <TriplanarGradientMap.hpp>
#include <TransformHelper.hpp>

#include <boost/variant.hpp>

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>


typedef boost::variant<noob::triplanar_renderer::uniform_info> render_info;

namespace noob
{
	class scene
	{
		public:
			// Built on-stack to provide easy access to a number of convenient variables
			struct node
			{
					node() : drawable_id(0), ready_to_draw(false) {}
					size_t drawable_id;
					noob::transform_helper xform;
					render_info drawing_info;
					bool ready_to_draw;
			};
		
			void init();
			void loop();
			void draw();
			bool add_to_scene(const noob::scene::node& node_to_add, const std::string& node_name, const std::string& parent_name = "");

			noob::scene::node get_node(const std::string& name) const;


			noob::physics_world physics;
		protected:
			void dynamic_graph_from_static();
			void static_graph_from_dynamic();

			lemon::ListDigraph dynamic_graph;
			lemon::StaticDigraph static_graph;

			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;

			std::vector<noob::mat4> view_matrices;
			

			std::map<const std::string, int> node_names;
	};
}
