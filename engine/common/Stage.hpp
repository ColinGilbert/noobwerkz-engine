// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <functional>

#include <btBulletDynamicsCommon.h>

#include "NoobDefines.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "BasicRenderer.hpp"
#include "TriplanarGradientMap.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "ScaledModel.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "Body.hpp"
#include "Joint.hpp"
#include "Shape.hpp"
#include "Component.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"
#include "HandleMap.hpp"
#include "FastHashTable.hpp"
#include "Graph.hpp"

#include <standalone/brigand.hpp>

#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/lgf_writer.h>


namespace noob
{
	class stage
	{
		public:

			// First, we declare a few structs.
			struct contact_point
			{
				size_t handle;
				noob::vec3 pos_a, pos_b, normal_on_b;
			};
			
			struct ghost_intersection_results
			{
				noob::ghost_handle ghost;
				std::vector<noob::body_handle> bodies;
				std::vector<noob::ghost_handle> ghosts;
			};

			// We'll happen to use these members a lot.
			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			bool show_origin;

			noob::vec4 ambient_light;

			// Now, onto the class itself.

			stage() : show_origin(true), ambient_light(noob::vec4(0.1, 0.1, 0.1, 0.1)), bodies_mapping(draw_graph), model_mats_mapping(draw_graph), basic_models_mapping(draw_graph), shaders_mapping(draw_graph), reflectances_mapping(draw_graph), scales_mapping(draw_graph), lights_mapping(draw_graph), ghosts_initialized(false) {}

			~stage();

			// This one must be called by the application.
			void init();
		
			// Brings everything back to scratch.
			void tear_down();

			// Call those every frame or so.
			void update(double dt);
			
			void draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) const;

			// Creates physics body. Those get made lots.
			noob::body_handle add_body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0), bool ccd = false);
			noob::ghost_handle add_ghost(const noob::shape_handle, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0));

			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b);
			
			// Functions to create commonly-used configurations. Soon they'll return a tag used by the component system (in construction)
			void actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const noob::globals::shader_results);
			
			void actor(const noob::bodies_holder::handle, const noob::scaled_model, const noob::globals::shader_results);
			// void actor(const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::scaled_model, const noob::globals::shader_results);
			void actor(const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results, const noob::reflectances_holder::handle);
			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			void scenery(const noob::basic_mesh&, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results, const noob::reflectances_holder::handle, const std::string& name);

			void set_light(unsigned int, const noob::lights_holder::handle);

			void set_directional_light(const noob::directional_light&);

			noob::lights_holder::handle get_light(unsigned int i) const;
			
			noob::stage::ghost_intersection_results get_intersections(const noob::ghost_handle) const;
			
			// Dumps a readable graph format onto disk. Super useful for debug.
			void write_graph(const std::string& filename) const;

			

		protected:
			
			void remove_body(noob::body_handle);
			
			const int NUM_RESERVED_NODES = 8192;			
			const int NUM_RESERVED_ARCS = 8192;

			noob::dynamic_graph ng_graph;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::Node root_node;
			lemon::ListDigraph::NodeMap<size_t> bodies_mapping;
			lemon::ListDigraph::NodeMap<std::function<noob::mat4(void)>> model_mats_mapping;
			lemon::ListDigraph::NodeMap<size_t> basic_models_mapping;
			lemon::ListDigraph::NodeMap<noob::globals::shader_results> shaders_mapping;
			lemon::ListDigraph::NodeMap<size_t> reflectances_mapping;
			lemon::ListDigraph::NodeMap<std::array<size_t, 4>> lights_mapping;
			lemon::ListDigraph::NodeMap<std::array<float, 3>> scales_mapping;

			// std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
			noob::fast_hashtable bodies_to_nodes;
			noob::fast_hashtable basic_models_to_nodes;
			// std::map<size_t, lemon::ListDigraph::Node> shaders_to_nodes;
			noob::directional_light directional_light;
			std::array<noob::lights_holder::handle, MAX_LIGHTS> lights;

			bool ghosts_initialized;
	};
}
