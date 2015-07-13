// This class uses two graphs: One for modifying and another for super-fast iterating.
#pragma once

#include <MathFuncs.hpp>
#include <VoxelWorld.hpp>
#include <HACDRenderer.hpp>
#include <TriplanarGradientMap.hpp>
#include <TransformHelper.hpp>

#include <boost/variant.hpp>
#include <boost/intrusive_ptr.hpp>

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>


typedef boost::variant<noob::triplanar_renderer::uniform_info> render_info;

namespace noob
{
	class scene
	{
		public:
			class node
			{
				public:
					node() : drawable_id(0), name(""), ready_for_drawing(false) {}
					size_t id;
					size_t drawable_id;
					noob::transform_helper xform;
					std::string name;	

				protected:
					std::tuple<size_t, render_info> renderpair;
					std::vector<std::tuple<const std::string, size_t>> name_index;
					bool ready_for_drawing;
			};
		
			void init();
			void loop();
			void draw();
			bool add_to_scene(const noob::scene::node& node_to_add, const std::string& parent_name = "");

			// TODO: Find a safer manner to do this. 
			noob::scene::node get_node(const std::string& name) const;


		protected:
			void dynamic_graph_from_static();
			void static_graph_from_dynamic();

			lemon::ListDigraph dynamic_graph;
			lemon::StaticDigraph static_graph;

			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;

			std::vector<noob::mat4> view_matrices;


	};

}
