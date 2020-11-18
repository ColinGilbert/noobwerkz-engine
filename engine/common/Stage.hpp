// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <functional>

#include <noob/fast_hashtable/fast_hashtable.hpp>
#include <noob/graph/graph.hpp>
#include <noob/component/component.hpp>
#include <noob/math/math_funcs.hpp>

#include "Graphics.hpp"
#include "SkeletalAnim.hpp"
#include "Physics.hpp"
#include "Body.hpp"
#include "Constraint.hpp"
#include "Shape.hpp"
#include "Actor.hpp"
#include "StageItemType.hpp"
#include "Scenery.hpp"
#include "Globals.hpp"
#include "Particles.hpp"
#include "Armature.hpp"
#include "NDOF.hpp"
#include "Prop.hpp"
#include "Assembly.hpp"


namespace noob
{
	class stage
	{
		public:
			stage() noexcept(true) : show_origin(true), ambient_light(noob::vec4f(0.6, 0.6, 0.6, 1.0)) {}

			// This one must be called by the application.
			void init(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true);
			// Brings everything back to scratch.
			void tear_down() noexcept(true);

			// It is recommended that the main stage gets both update() and draw() called each frame.
			// It is also feasible to have other stages running alongside each other:
			// For example, the AI could simulate a tree of possible events and choose the best solution prior to making its decision.
			// Also, one stage could be preloading an updated scene (from file, network, or procedural content) prior to being swapped to being currently-displayed stage.
			// Drawing can also feasibly be called more than once per frame, especially after setting the viewport to a different FOV/LOD. The results can then be treated as a texture and displayed within a game, or on a HUD.
			void update(double dt) noexcept(true);
			// TODO: REMOVE DRAW FUNCTION!
			void draw() noexcept(true);
			void update_viewport_params(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true);

			// Call this when your graphics context went dead and is now back (ie: app got sent into background)
			void rebuild_graphics(const noob::vec2ui& dims, const noob::mat4f& projection_mat) noexcept(true);
			void rebuild_models() noexcept(true);
			void build_navmesh() noexcept(true);

			noob::actor_blueprints_handle add_actor_blueprints(const noob::actor_blueprints&) noexcept(true);
			noob::prop_blueprints_handle add_props_blueprints(const noob::prop_blueprints&) noexcept(true);;

			void reserve_actors(const noob::actor_blueprints_handle, uint32_t num) noexcept(true);
			void reserve_props(const noob::prop_blueprints_handle, uint32_t num) noexcept(true);

			void set_team_colour(uint32_t team_num, const noob::vec4f& colour) noexcept(true);

			noob::actor_handle create_actor(const noob::actor_blueprints_handle, uint32_t team, const noob::vec3f&, const noob::versorf&) noexcept(true);
			noob::prop_handle create_prop(const noob::prop_blueprints_handle, uint32_t team, const noob::vec3f&, const noob::versorf&) noexcept(true);
			noob::scenery_handle create_scenery(const noob::shape_handle, const noob::vec3f&, const noob::versorf&) noexcept(true);
			noob::assembly_handle create_assembly(const noob::vec3f&, const noob::versorf&, bool ccd, const std::vector<noob::prop_handle> &, const std::vector<noob::compound_shape::child_info>&) noexcept(true);


			noob::scenery& get_scenery(const noob::scenery_handle) noexcept(true);
			noob::actor& get_actor(const noob::actor_handle) noexcept(true);
			noob::prop& get_prop(const noob::prop_handle) noexcept(true);

			noob::body& get_body(const noob::body_handle) noexcept(true);

			noob::constraint& get_constraint(const noob::constraint_handle) noexcept(true);		
			noob::constraint_handle create_fixed_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& frame_in_a, const noob::mat4f& frame_in_b) noexcept(true);
			noob::constraint_handle create_point_constraint(const noob::body_handle a, const noob::body_handle b, const noob::vec3f& pivot_a, const noob::vec3f& pivot_b) noexcept(true);
			noob::constraint_handle create_hinge_constraint(const noob::body_handle a, const noob::body_handle b, const noob::vec3f& pivot_in_a, const noob::vec3f& pivot_in_b, const noob::vec3f& axis_in_a, const noob::vec3f& axis_in_b) noexcept(true);
			noob::constraint_handle create_slide_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);
			// noob::constraint_handle create_conical_constraint() noexcept(true);
			// noob::constraint_handle create_gear_constraint() noexcept(true);
			noob::constraint_handle create_generic_constraint(const noob::body_handle a, const noob::body_handle b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);
			
			void set_actor_position(const noob::actor_handle, const noob::vec3f&) noexcept(true);
			void set_actor_orientation(const noob::actor_handle, const noob::versorf&) noexcept(true);
			void set_actor_velocity(const noob::actor_handle, const noob::vec3f&) noexcept(true);
			void set_actor_target(const noob::actor_handle, const noob::vec3f&) noexcept(true);
			void set_actor_self_controlled(const noob::actor_handle, bool) noexcept(true);
			void set_actor_team(const noob::actor_handle, uint32_t) noexcept(true);

			noob::vec3f get_actor_position(const noob::actor_handle) noexcept(true);
			noob::versorf get_actor_orientation(const noob::actor_handle) noexcept(true);
			noob::vec3f get_actor_velocity(const noob::actor_handle) noexcept(true);
			noob::vec3f get_actor_target(const noob::actor_handle) noexcept(true);
			bool get_actor_self_controlled(const noob::actor_handle) noexcept(true);
			uint32_t get_actor_team(const noob::actor_handle) noexcept(true);

			size_t get_intersecting(const noob::actor_handle, std::vector<noob::contact_point>&) const noexcept(true);

			// TODO: Methodize
			bool show_origin;

			// TODO: Make more flexible.
			noob::vec4f ambient_light;

			std::string print_drawables_info() const noexcept(true);

			// TODO: Remove
			void accept_ndof_data(const noob::ndof::data& info) noexcept(true);


		protected:
			struct drawable_instance
			{
				enum class type { ACTOR = 0, PROP = 1 };
				noob::stage::drawable_instance::type which;
				uint32_t index;
				// uint32_t part, pose;
			};

			struct drawable_info
			{
				noob::instanced_model_handle model;
				uint32_t count = 0;
				bool needs_colours;
				std::vector<drawable_instance> instances;
			};
			
			struct actor_info
			{
				noob::actor_blueprints bp;
				uint32_t count, max;
			};
			
			struct prop_info
			{
				noob::prop_blueprints bp;
				uint32_t count, max;
			};

			static constexpr auto dbg_name = "Stage";

			bool terrain_changed = false;
			bool terrain_started = false;
			// bool graphics_enabled = false; // TODO: Implement conditional checks

			noob::physics world;

			noob::mat4f view_matrix, projection_matrix;
			noob::vec2ui viewport_dims;
			uint32_t num_terrain_verts = 0;

			noob::directional_light main_light;

			std::vector<noob::stage::drawable_info> drawables;
			std::vector<noob::stage::actor_info> actors_extra_info;
			std::vector<noob::stage::prop_info> props_extra_info;
			std::vector<noob::vec4f> team_colours;
			// Voronoi cells are preferred over convex hulls generated from a set of points for data representation, as we can be certain that the voronoi cell is unambiguous.
			// rde::vector<noob::vorocell> voros;

			noob::fast_hashtable models_to_drawable_instances;
			noob::fast_hashtable shapes_to_instanced_models;

			std::vector<noob::actor> actors;
			std::vector<noob::prop> props;
			std::vector<noob::scenery> sceneries;
			std::vector<noob::assembly> assemblies;
			std::vector<noob::particle_system> particle_systems;
			std::vector<noob::contact_point> contacts; // Our holder for temporary contact points; this limits us to single-threaded use and it thus might be worth exploring a multithreaded solution.

			noob::component_dynamic<noob::armature> armatures;

			noob::duration update_duration;
			noob::duration draw_duration;
			noob::duration last_navmesh_build_duration;

			bool ndof_happened = false;
			noob::vec3f ndof_translation, ndof_rotation;			

			void run_ai() noexcept(true);

			// rde::slist<rde::vector<noob::vec3f>> paths;

			void update_actors() noexcept(true);

			void actor_dither(const noob::actor_handle) noexcept(true);
			typedef noob::handle<drawable_info> drawable_info_handle;

			void upload_colours(const drawable_info_handle) const noexcept(true);
			void upload_matrices(const drawable_info_handle) noexcept(true);

			// This method checks to see if there have been any models of this type reserved prior to reserving them and reserves + allocates if not. If anything *is* reserved, it'll still only allocate if Num > originally allocated.
			void reserve_models(const noob::instanced_model_handle h, uint32_t Num) noexcept(true);

			void upload_terrain() noexcept(true);

			noob::body_handle create_body(const noob::shape_handle, const noob::body_info) noexcept(true); 
	};
}
