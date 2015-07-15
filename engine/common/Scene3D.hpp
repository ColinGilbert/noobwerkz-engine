// This class uses two graphs: One for modifying and another for super-fast iterating.
#pragma once
#include "Config.hpp"
#include <PhysicsWorld.hpp>
#include <Graphics.hpp>
#include <MathFuncs.hpp>
#include <VoxelWorld.hpp>
#include <HACDRenderer.hpp>
#include <TriplanarGradientMap.hpp>
#include <TransformHelper.hpp>
#include <unordered_map>
#include <string>
#include <boost/variant.hpp>

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>


namespace noob
{
	class scene3d
	{
		public:
			typedef boost::variant<noob::triplanar_renderer::uniform_info> shader_uniforms;
			//typedef boost::variant<const btRigidBody*, const noob::drawable3d*> node_item;

			class node
			{
				public:
					node() : visible(true), synced(false), body_id(0), drawable_id(0), matrix(noob::identity_mat4()) {}
					
					noob::mat4 get_matrix() const { return matrix; }
					
					void set_matrix(const noob::mat4& m) {matrix = m; }
					
					void tick()
					{
						if (synced == false)
						{	

							// const btMotionState* motion_state = body->getMotionState();
							// btTransform bullet_transform;
							// motion_state->getWorldTransform(bullet_transform);
							// bullet_transform.getOpenGLMatrix(&matrix[0]);
						}
						synced = true;
					}

					void draw()
					{
						if (visible)
						{
							
						}
					}

				protected:
					bool visible, synced;
					size_t body_id;;
					size_t drawable_id;
					noob::mat4 matrix;
					noob::scene3d::shader_uniforms uniform;

			};

			scene3d() : node_counter(0) {} //d_nodes(dynamic_graph), s_nodes(static_graph) {}
			void init();
			void loop();
			void draw();
			
			size_t add(const noob::scene3d::node& node_to_add, bool active = true);

			size_t get_id(const std::string& name) const;
			noob::scene3d::node get(size_t) const;

			void activate(const std::vector<size_t>& node_ids);
			void disactivate(const std::vector<size_t>& node_ids);


			noob::physics_world physics;

		protected:
			// void dynamic_graph_from_static();
			// void static_graph_from_dynamic();

			// lemon::ListDigraph dynamic_graph;
			// lemon::StaticDigraph static_graph;

			// lemon::ListDigraph::NodeMap<noob::scene3d::node> d_nodes;
			// lemon::StaticDigraph::NodeMap<noob::scene3d::node> s_nodes;

			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;

			std::vector<noob::mat4> view_matrix;

			std::vector<size_t> active_nodes; //noob::scene3d::node> active_nodes;
			std::unordered_map<std::string, size_t> node_names;
			std::unordered_map<size_t, noob::scene3d::node> nodes;
			size_t node_counter;
	};
}
