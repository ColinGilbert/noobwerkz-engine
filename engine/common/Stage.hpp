// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <functional>

#include <rdestl/slist.h>
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
#include "DynamicArray.hpp"
#include "ShaderVariant.hpp"
#include "ContactPoint.hpp"
#include "StageTypes.hpp"
#include "Particles.hpp"
// #include "NavMesh.hpp"

#include "Graph.hpp"
#include "Concatenate.hpp"

namespace noob
{
	class stage
	{
		public:

			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4(0.1, 0.1, 0.1, 0.1)), instancing(false) {}

			~stage() noexcept(true);

			// This one must be called by the application.
			void init() noexcept(true);

			// Brings everything back to scratch.
			void tear_down() noexcept(true);

			// It is recommended that the main stage gets both update() and draw() called each frame.
			// It is also feasible to have other stages running alongside each other:
			// For example, the AI could simulate a tree of possible events and choose the best solution prior to making its decision.
			// Also, one stage could be preloading an updated scene (from file, network, or procedural content) prior to being swapped to being currently-displayed stage.
			// Draw() can also feasibly be called more than once per frame, especially after setting the viewport to a different FOV/LOD. The results can then be treated as a texture and displayed within a game, or on a HUD.
			void update(double dt) noexcept(true);

			void draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) noexcept(true);

			void build_navmesh() noexcept(true);

			// Creates physics body, sensors, joints. Those get made lots.
			noob::body_handle body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0), bool ccd = false) noexcept(true);

			noob::ghost_handle ghost(const noob::shape_handle, const noob::vec3& pos, const noob::versor& orient = noob::versor(1.0, 0.0, 0.0, 0.0)) noexcept(true);

			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true);

			// These are the composites that use the bodies, ghosts, and joints.
			noob::actor_handle actor(const noob::actor_blueprints_handle, uint32_t team, const noob::vec3&, const noob::versor&);

			noob::scenery_handle scenery(const noob::shape_handle shape_arg, const noob::shader_variant shader_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg);

			// Lights.
			void set_light(unsigned int, const noob::light_handle) noexcept(true);

			void set_directional_light(const noob::directional_light&) noexcept(true);

			noob::light_handle get_light(uint32_t) const noexcept(true);


			// Intersection enumeration.
			std::vector<noob::contact_point> get_intersecting(const noob::ghost_handle) const noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::actor_handle) const noexcept(true);

			// Dumps a readable graph format onto disk. Super useful for debug.
			// void write_graph(const std::string& filename) const noexcept(true);

			bool show_origin;

			noob::vec4 ambient_light;

			void set_instancing(bool b) noexcept(true)
			{
				instancing = b;
			}

		protected:

			static constexpr auto dbg_name = "Stage";

			void run_ai() noexcept (true);

			// rde::slist<rde::vector<noob::vec3>> paths;

			void remove_body(noob::body_handle) noexcept(true);
			void remove_ghost(noob::ghost_handle) noexcept(true);
			// void remove_joint(noob::joint_handle) noexcept(true);

			void update_actors() noexcept(true);

			void actor_dither(noob::actor_handle h) noexcept(true);

			uint32_t add_to_graph(const noob::handle<noob::stage_item_variant>, const noob::shader_variant, const noob::shape_handle, const noob::reflectance_handle) noexcept(true); 


			// void update_particle_systems() noexcept(true);
			// void particle_spawn_helper(noob::particle_system*) noexcept(true); 

			noob::graph draw_graph;

			noob::duration update_duration;
			noob::duration draw_duration;
			noob::duration last_navmesh_build_duration;

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
			basic_shaders_holder particle_shaders;

			noob::component<noob::stage_item_variant> stage_item_variants;
			// noob::component<noob::shader_variant> bod_shaders;
			// noob::component<noob::vec3> bod_scales;
			

			rde::vector<noob::actor_event> actor_mq;
			uint32_t actor_mq_count;

			bool instancing;

			// For drawing
			// noob::fast_hashtable bodies_to_nodes;
			// noob::fast_hashtable ghosts_to_nodes;
			// noob::fast_hashtable basic_models_to_nodes;

			noob::directional_light directional_light;
			
			// noob::navigation nav;
			// bool nav_changed;
			// TODO: Make more flexible.
			std::array<noob::light_handle, MAX_LIGHTS> lights;
	};
}
