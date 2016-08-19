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
#include "ContactPoint.hpp"
#include "StageTypes.hpp"
#include "NoobCommon.hpp"
#include "Particles.hpp"


#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/lgf_writer.h>


namespace noob
{
	class stage
	{
		public:
			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4(0.1, 0.1, 0.1, 0.1)), instancing(true), bodies_mapping(draw_graph), /* model_mats_mapping(draw_graph),*/ basic_models_mapping(draw_graph), shaders_mapping(draw_graph), reflectances_mapping(draw_graph), scales_mapping(draw_graph), lights_mapping(draw_graph), matrix_pool_count(0) {}

			~stage() noexcept(true);

			// This one must be called by the application.
			void init() noexcept(true);

			// Brings everything back to scratch.
			void tear_down() noexcept(true);

			// Call those every frame or so.
			void update(double dt) noexcept(true);

			void draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) noexcept(true);

			// Creates physics body, sensors, joints. Those get made lots.
			noob::body_handle body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0), bool ccd = false) noexcept(true);

			noob::ghost_handle ghost(const noob::shape_handle, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0)) noexcept(true);

			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true);

			// These are the composites that use the bodies, ghosts, and joints.
			noob::actor_handle actor(const noob::actor_blueprints_handle, uint32_t team, const noob::vec3&, const noob::versor&);

			noob::scenery_handle scenery(const noob::shape_handle shape_arg, const noob::shader shader_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg);

			noob::particle_system_handle particle_system(const noob::particle_system_descriptor&);

			void set_light(unsigned int, const noob::light_handle) noexcept(true);

			void set_directional_light(const noob::directional_light&) noexcept(true);

			noob::light_handle get_light(uint32_t) const noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::ghost_handle) const noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::actor_handle) const noexcept(true);

			// Dumps a readable graph format onto disk. Super useful for debug.
			// void write_graph(const std::string& filename) const noexcept(true);

			bool show_origin;

			noob::vec4 ambient_light;

			void set_instancing(bool b)
			{
				instancing = b;
			}

		protected:

			void run_ai() noexcept (true);

			void remove_body(noob::body_handle) noexcept(true);
			void remove_ghost(noob::ghost_handle) noexcept(true);
			// void remove_joint(noob::joint_handle) noexcept(true);

			void add_to_graph(const noob::body_variant bod_arg, const noob::shape_handle shape_arg, const noob::shader shader_arg, const noob::reflectance_handle reflect_arg); 

			void actor_dither(noob::actor_handle h) noexcept(true);

			void update_particle_systems() noexcept(true);

			const int NUM_RESERVED_NODES = 8192;			
			const int NUM_RESERVED_ARCS = 8192;

			noob::duration update_duration;
			noob::duration draw_duration;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			noob::component<noob::actor> actors;
			noob::component<noob::scenery> sceneries;
			noob::component<noob::particle_system> particle_systems;

			rde::vector<noob::actor_event> actor_mq;
			uint32_t actor_mq_count;

			bool instancing;

			// TODO: Optimize:
			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::Node root_node;
			lemon::ListDigraph::NodeMap<noob::body_variant> bodies_mapping;
			// lemon::ListDigraph::NodeMap<std::function<noob::mat4(void)>> model_mats_mapping;
			lemon::ListDigraph::NodeMap<uint32_t> basic_models_mapping;
			lemon::ListDigraph::NodeMap<noob::shader> shaders_mapping;
			lemon::ListDigraph::NodeMap<uint32_t> reflectances_mapping;
			lemon::ListDigraph::NodeMap<std::array<uint32_t, 4>> lights_mapping;
			lemon::ListDigraph::NodeMap<std::array<float, 3>> scales_mapping;

			rde::vector<noob::mat4> matrix_pool;
			uint32_t matrix_pool_count;
			static const uint32_t matrix_pool_stride = 2;

			// For drawing
			noob::fast_hashtable bodies_to_nodes;
			noob::fast_hashtable ghosts_to_nodes;
			noob::fast_hashtable basic_models_to_nodes;

			noob::directional_light directional_light;

			// TODO: Make more flexible.
			std::array<noob::light_handle, MAX_LIGHTS> lights;
	};
}
