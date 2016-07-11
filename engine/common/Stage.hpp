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


			bool show_origin;

			noob::vec4 ambient_light;

			// Now, onto the class itself.

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
			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true);
			
			// Functions to create commonly-used configurations. Soon they'll return a tag used by the component system (in construction)
			void actor(const noob::body_handle, const noob::animated_model_handle, const noob::globals::shader_results) noexcept(true);
			
			void actor(const noob::body_handle, const noob::scaled_model, const noob::globals::shader_results) noexcept(true);
			// void actor(const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::scaled_model, const noob::globals::shader_results) noexcept(true);
			void actor(const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results, const noob::reflectance_handle) noexcept(true);
			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			void scenery(const noob::basic_mesh&, const noob::vec3& pos, const noob::versor& orient, const noob::globals::shader_results, const noob::reflectance_handle, const std::string& name) noexcept(true);

			void set_light(unsigned int, const noob::light_handle) noexcept(true);

			void set_directional_light(const noob::directional_light&) noexcept(true);

			noob::light_handle get_light(unsigned int i) const noexcept(true);
			
			noob::stage::ghost_intersection_results get_intersections(const noob::ghost_handle) const noexcept(true);
			
			// Dumps a readable graph format onto disk. Super useful for debug.
			void write_graph(const std::string& filename) const noexcept(true);

			

		protected:
			
			void remove_body(noob::body_handle) noexcept(true);
			
			const int NUM_RESERVED_NODES = 8192;			
			const int NUM_RESERVED_ARCS = 8192;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// We'll happen to use these members a lot.
			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			// In effect, an actor is a token encoded with the necessary info to get world position + models.
			class actor_info
			{
				friend class stage;

				public:
				actor_info() noexcept(true) : debug(false), index(std::numeric_limits<uint32_t>::max()), shader(std::numeric_limits<uint32_t>::max()) {}
				
				protected:

				// An explanation of the code paths taken for each type:
				// BODY gets its position from bodies[n]. It is associated with its shape. That and the shader info determines how and where to draw it.
				// GHOST gets drawn the same, except its position is drawn from ghosta[n].
				// Note For ARMATURE and SKELETAL actor types: If debug is set, the bounding volume gets drawn.
				// ARMATURE refers to a set of basic models being drawn hierarchically. This actor's position/orientation becomes that of the root node.
				// SKELETAL refers to a mesh with bone weights being drawn along with skinning matrices. This actor's position/orientation becomes that of the root node.
				// The only real difference in how these are treated is that a rigid armature can be drawn more quickly than skeletal animations.
				// Under the hood, either the armature actors or skeletal actors can be driven by an active rig, a set of prebaked matrices, an IK effector - or an arbitrary combination of all these things!
				// In fact, a skeletal animation's skinning matrices often start their lives as armatures which get exported to canned animations via ozz-anim's excellent toolkit. This can even be done at runtime if desired.
				enum class type { BODY = 0, GHOST = 1, ARMATURE = 2, SKELETAL = 3 };
				enum class shading_type { BASIC = 0, TRIPLANAR = 1 };
			
				// The shader of a multipart actor root is for debugging. We can safely skip it if we're drawing normally, as the other actors in the armature contain the relevant shading info.
				bool debug;
				positioning_type type;
				shading_type shading;
				uint32_t index, shader;
			};

			typedef noob::component<actor_info> actors_holder;


			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::Node root_node;
			lemon::ListDigraph::NodeMap<size_t> bodies_mapping;
			lemon::ListDigraph::NodeMap<std::function<noob::mat4(void)>> model_mats_mapping;
			lemon::ListDigraph::NodeMap<size_t> basic_models_mapping;
			lemon::ListDigraph::NodeMap<noob::globals::shader_results> shaders_mapping;
			lemon::ListDigraph::NodeMap<size_t> reflectances_mapping;
			lemon::ListDigraph::NodeMap<std::array<size_t, 4>> lights_mapping;
			lemon::ListDigraph::NodeMap<std::array<float, 3>> scales_mapping;

			noob::fast_hashtable bodies_to_nodes;
			noob::fast_hashtable basic_models_to_nodes;
			noob::directional_light directional_light;
			std::array<noob::light_handle, MAX_LIGHTS> lights;

			bool ghosts_initialized;
	};
}
