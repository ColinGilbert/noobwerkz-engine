// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

// #include <stack>
// #include <string>
// #include <tuple>
// #include <list>
// #include <forward_list>
#include <functional>

#include <btBulletDynamicsCommon.h>
// #include <dcollide.h>

#include "NoobDefines.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
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
// #include "IntrusiveBase.hpp"
#include "ComponentDefines.hpp"

#include <standalone/brigand.hpp>

#include <lemon/smart_graph.h>
#include <lemon/list_graph.h>
#include <lemon/lgf_writer.h>

namespace noob
{
	class stage
	{
		public:
			stage() : show_origin(true), view_mat(noob::identity_mat4()), projection_mat(noob::identity_mat4()), ambient_light(noob::vec4(0.1, 0.1, 0.1, 0.1)), eye_pos(noob::vec3(0.0, 0.0, 0.0)), bodies_mapping(draw_graph), model_mats_mapping(draw_graph),  basic_models_mapping(draw_graph), shaders_mapping(draw_graph), reflectances_mapping(draw_graph), scales_mapping(draw_graph), lights_mapping(draw_graph) {}

			~stage();

			// This one must be called by the application. It really sucks but that's because the graphics API is (currently) static. This may well change soon enough.
			void init(const noob::globals*);

			// This one provides a way to bring everything back to scratch 
			void tear_down();

			// Call those every frame or so.
			void update(double dt);
			// TODO: Implement
			void draw(float window_width, float window_height) const;

			// Creates physics body. Those get made lots.
			noob::body_handle body(const noob::body_type, const noob::shape_handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);

			noob::joint_handle joint(const noob::body_handle a, const noob::vec3& point_on_a, const noob::body_handle b, const noob::vec3& point_on_b);
			
			noob::bodies_holder bodies;
			noob::joints_holder joints;
			noob::ghosts_holder ghosts;

			// Functions to create commonly-used configurations:
			void actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const noob::shaders_holder::handle);

			void actor(const noob::bodies_holder::handle, const noob::scaled_model, const noob::shaders_holder::handle);

			// void actor(const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::scaled_model, const noob::shaders_holder::handle);
			void actor(const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::shaders_holder::handle, const noob::reflectances_holder::handle);

			// void actor(const noob::shapes_holder::handle, const noob::vec3& pos, const noob::vec3& orient, const noob::vec3& scale, const noob::shaders_holder::handle);

			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			void scenery(const noob::basic_mesh&, const noob::vec3& pos, const noob::versor& orient, const noob::shaders_holder::handle, const noob::reflectances_holder::handle, const std::string& name);

			void set_light(unsigned int, const noob::lights_holder::handle);

			noob::lights_holder::handle get_light(unsigned int i) const;

			void write_graph(const std::string& filename) const;

			bool show_origin;

			noob::mat4 view_mat, projection_mat;
			
			noob::vec4 ambient_light;
			noob::vec3 eye_pos;

			// noob::prepared_shaders renderer;


		protected:
			const int NUM_RESERVED_NODES = 12000;			
			const int NUM_RESERVED_ARCS = 12000;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::NodeMap<size_t> bodies_mapping;
			lemon::ListDigraph::NodeMap<std::function<noob::mat4(void)>> model_mats_mapping;
			lemon::ListDigraph::NodeMap<size_t> basic_models_mapping;
			lemon::ListDigraph::NodeMap<size_t> shaders_mapping;
			lemon::ListDigraph::NodeMap<size_t> reflectances_mapping;
			lemon::ListDigraph::NodeMap<std::array<size_t, 4>> lights_mapping;
			lemon::ListDigraph::NodeMap<std::array<float, 3>> scales_mapping;
			lemon::ListDigraph::Node root_node;

			// std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
			std::map<size_t, lemon::ListDigraph::Node> bodies_to_nodes;
			std::map<size_t, lemon::ListDigraph::Node> basic_models_to_nodes;
			// std::map<size_t, lemon::ListDigraph::Node> shaders_to_nodes;

			std::array<noob::lights_holder::handle, MAX_LIGHTS> lights;

			noob::globals* globals;
	};
}
