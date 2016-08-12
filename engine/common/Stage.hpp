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
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "ScaledModel.hpp"
#include "AnimatedModel.hpp"
#include "Body.hpp"
#include "Joint.hpp"
#include "Shape.hpp"
#include "Armature.hpp"
#include "Actor.hpp"
#include "Component.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"
#include "HandleMap.hpp"
#include "FastHashTable.hpp"
#include "NoobCommon.hpp"
#include "DynamicArray.hpp"
#include "ShadingVariant.hpp"

#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/lgf_writer.h>


namespace noob
{
	class stage
	{
		public:
			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4(0.1, 0.1, 0.1, 0.1)), bodies_mapping(draw_graph), model_mats_mapping(draw_graph), basic_models_mapping(draw_graph), shaders_mapping(draw_graph), reflectances_mapping(draw_graph), scales_mapping(draw_graph), lights_mapping(draw_graph), ghosts_initialized(false) {}

			~stage() noexcept(true);

			// This one must be called by the application.
			void init() noexcept(true);

			// Brings everything back to scratch.
			void tear_down() noexcept(true);

			// Call those every frame or so.
			void update(double dt) noexcept(true);

			void draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) const noexcept(true);

			// Creates physics body, sensors, joints. Those get made lots.
			noob::body_handle add_body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0), bool ccd = false) noexcept(true);
			noob::ghost_handle add_ghost(const noob::shape_handle, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0)) noexcept(true);
			noob::joint_handle add_joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true);

			typedef noob::handle<noob::actor> actor_handle;

			noob::actor_handle add_actor(const noob::actor_blueprints_handle, uint32_t team, const noob::vec3&, const noob::versor&);

			
			void scenery(const noob::shape_handle shape_arg, const noob::shader shader_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg);
			
			void set_light(unsigned int, const noob::light_handle) noexcept(true);

			void set_directional_light(const noob::directional_light&) noexcept(true);

			noob::light_handle get_light(unsigned int i) const noexcept(true);

			noob::ghost_intersection_results get_intersections(const noob::ghost_handle) const noexcept(true);

			// Dumps a readable graph format onto disk. Super useful for debug.
			void write_graph(const std::string& filename) const noexcept(true);

			bool show_origin;

			noob::vec4 ambient_light;


		protected:
			void remove_body(noob::body_handle) noexcept(true);

			void add_to_graph(const noob::body_variant bod_arg, const noob::shape_handle shape_arg, const noob::shader shader_arg, const noob::reflectance_handle reflect_arg); 

			const int NUM_RESERVED_NODES = 8192;			
			const int NUM_RESERVED_ARCS = 8192;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			noob::component<noob::actor> actors;
			
			rde::vector<noob::actor_event> actor_mq;
			uint32_t actor_mq_count;
			

			// TODO: Optimize:
			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::Node root_node;
			lemon::ListDigraph::NodeMap<noob::body_variant> bodies_mapping;
			lemon::ListDigraph::NodeMap<std::function<noob::mat4(void)>> model_mats_mapping;
			lemon::ListDigraph::NodeMap<uint32_t> basic_models_mapping;
			lemon::ListDigraph::NodeMap<noob::shader> shaders_mapping;
			lemon::ListDigraph::NodeMap<uint32_t> reflectances_mapping;
			lemon::ListDigraph::NodeMap<std::array<uint32_t, 4>> lights_mapping;
			lemon::ListDigraph::NodeMap<std::array<float, 3>> scales_mapping;

			noob::fast_hashtable bodies_to_nodes;
			noob::fast_hashtable ghosts_to_nodes;
			noob::fast_hashtable basic_models_to_nodes;
			
			noob::directional_light directional_light;
			
			// TODO: Make more flexible.
			std::array<noob::light_handle, MAX_LIGHTS> lights;

			bool ghosts_initialized;
	};
}
