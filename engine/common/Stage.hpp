// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <functional>

#include <rdestl/slist.h>
// #include <btBulletDynamicsCommon.h>
#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Graphics.hpp"
#include "SkeletalAnim.hpp"
#include "Physics.hpp"
#include "Body.hpp"
#include "Joint.hpp"
#include "Shape.hpp"
#include "Actor.hpp"
#include "StageItemType.hpp"
#include "Scenery.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"
#include "Particles.hpp"
#include "Armature.hpp"
#include "NDOF.hpp"


namespace noob
{
	class stage
	{
		public:
			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4(0.6, 0.6, 0.6, 1.0)) {}

			// This one must be called by the application.
			void init(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true);

			// Brings everything back to scratch.
			void tear_down() noexcept(true);

			// It is recommended that the main stage gets both update() and draw() called each frame.
			// It is also feasible to have other stages running alongside each other:
			// For example, the AI could simulate a tree of possible events and choose the best solution prior to making its decision.
			// Also, one stage could be preloading an updated scene (from file, network, or procedural content) prior to being swapped to being currently-displayed stage.
			// Draw() can also feasibly be called more than once per frame, especially after setting the viewport to a different FOV/LOD. The results can then be treated as a texture and displayed within a game, or on a HUD.
			void update(double dt) noexcept(true);

			void draw() noexcept(true);

			void update_viewport_params(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true);

			// Call this when your graphics context went dead and is now back (ie: app got sent into background)
			void rebuild_graphics(uint32_t width, uint32_t height, const noob::mat4& projection_mat) noexcept(true);

			void rebuild_models() noexcept(true);

			void build_navmesh() noexcept(true);

			noob::actor_blueprints_handle add_actor_blueprints(const noob::actor_blueprints&) noexcept(true);

			void reserve_actors(const noob::actor_blueprints_handle, uint32_t num) noexcept(true);

			noob::actor_handle actor(noob::actor_blueprints_handle, uint32_t team, const noob::vec3&, const noob::versor&) noexcept(true);

			void set_team_colour(uint32_t team_num, const noob::vec4& colour) noexcept(true);

			// noob::scenery_handle scenery(const noob::shape_handle shape_arg, const noob::reflectance_handle reflect_arg, const noob::vec3&, const noob::versor&) noexcept(true);

			std::vector<noob::contact_point> get_intersecting(const noob::actor_handle) const noexcept(true);

			bool show_origin;

			// TODO: Make more flexible.
			noob::vec4 ambient_light;
			noob::directional_light main_light;

			std::string print_drawables_info() const noexcept(true);

			void accept_ndof_data(const noob::ndof::data& info) noexcept(true);


		protected:
			struct drawable_instance
			{
				noob::actor_handle actor;
				uint32_t part, pose;
			};

			struct drawable_info
			{
				noob::model_handle model;
				uint32_t count;
				bool needs_colours;
				std::vector<drawable_instance> instances;
			};

			struct actor_info
			{
				noob::actor_blueprints bp;
				uint32_t count, max;
			};

			static constexpr auto dbg_name = "Stage";

			noob::physics world;

			noob::mat4 view_matrix, projection_matrix;
			uint32_t viewport_width, viewport_height;

			rde::vector<noob::stage::drawable_info> drawables;
			rde::vector<noob::stage::actor_info> actor_factories;

			rde::vector<noob::vec4> team_colours;

			noob::fast_hashtable models_to_instances;

			noob::component<noob::actor> actors;
			noob::component<noob::scenery> sceneries;
			noob::component<noob::particle_system> particle_systems;
			noob::component_dynamic<noob::armature> armatures;

			noob::duration update_duration;
			noob::duration draw_duration;
			noob::duration last_navmesh_build_duration;


			void run_ai() noexcept(true);

			// rde::slist<rde::vector<noob::vec3>> paths;

			void update_actors() noexcept(true);

			void actor_dither(noob::actor_handle) noexcept(true);

			typedef noob::handle<drawable_info> drawable_info_handle;

			void upload_colours(drawable_info_handle) const noexcept(true);

			void upload_matrices(drawable_info_handle) noexcept(true);

			void reserve_models(noob::model_handle h, uint32_t num) noexcept(true);


	};
}
