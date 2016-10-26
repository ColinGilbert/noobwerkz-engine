// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <functional>

#include <rdestl/slist.h>
#include <btBulletDynamicsCommon.h>
#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>

#include "NoobDefines.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "Body.hpp"
#include "Joint.hpp"
#include "Shape.hpp"
#include "Actor.hpp"
#include "StageItemType.hpp"
#include "Scenery.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"
#include "ContactPoint.hpp"
#include "Particles.hpp"



namespace noob
{
	class stage
	{
		public:
			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4(0.6, 0.6, 0.6, 3.0)) {}

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

			void draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) const noexcept(true);

			void build_navmesh() noexcept(true);

			// Creates physics body, sensors, joints. Those get made lots.
			noob::body_handle body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true);

			noob::ghost_handle ghost(const noob::shape_handle, const noob::vec3& pos, const noob::versor& orient) noexcept(true);

			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b) noexcept(true);

			// These are the composites that use the bodies, ghosts, and joints.
			bool reserve_actors(const noob::actor_blueprints_handle, uint32_t num) noexcept(true);

			noob::actor_handle actor(noob::actor_blueprints_handle, uint32_t team, const noob::vec3&, const noob::versor&) noexcept(true);

			void set_team_colour(uint32_t team_num, const noob::vec4& colour) noexcept(true);
			// Scenery consists of a static object with its properties given in the argument. They get instanced.
			// noob::scenery_handle scenery(const noob::shape_handle shape_arg, const noob::reflectance_handle reflect_arg, const noob::vec3&, const noob::versor&) noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::actor_handle) const noexcept(true);

			// Dumps a readable graph format onto disk. Super useful for debug.
			// void write_graph(const std::string& filename) const noexcept(true);

			bool show_origin;

			noob::vec4 ambient_light;
			// TODO: Make more flexible.
			noob::light directional_light;

		protected:

			static constexpr auto dbg_name = "Stage";

			void run_ai() noexcept(true);

			// rde::slist<rde::vector<noob::vec3>> paths;

			void remove_body(noob::body_handle) noexcept(true);
			void remove_ghost(noob::ghost_handle) noexcept(true);
			// void remove_joint(noob::joint_handle) noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::ghost_handle) const noexcept(true);

			void update_actors() noexcept(true);

			void actor_dither(noob::actor_handle h) noexcept(true);

			noob::duration update_duration;
			noob::duration draw_duration;
			noob::duration last_navmesh_build_duration;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
	
			// This is to allow access into the 'items' vector (useful for uploading matrices into buffers.
			struct model_mapping
			{
				noob::model_handle handle;
				uint32_t offset, num;
			};

			rde::vector<noob::stage::model_mapping> models_to_items;
			rde::vector<noob::stage_item_variant> items;

			// These are for holding useful data.
			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			noob::component<noob::actor> actors;
			noob::component<noob::scenery> sceneries;
			noob::component<noob::particle_system> particle_systems;
			
			rde::vector<noob::vec4> team_colours;
		
		};
}
