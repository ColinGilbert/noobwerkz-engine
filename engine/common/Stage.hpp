// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once


#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>

#include <boost/variant.hpp>

#include <btBulletDynamicsCommon.h>
// #include <dcollide.h>

#include "es/storage.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "PreparedShaders.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"
#include "Body.hpp"
#include "Shape.hpp"
#include "Component.hpp"
#include "GlobalComponents.hpp"
// #include "IntrusiveBase.hpp"

#include <standalone/brigand.hpp>
#include <lemon/smart_graph.h>


namespace noob
{
	typedef noob::component<std::unique_ptr<noob::body>> bodies_holder;

	class stage
	{
		public:
			stage() : show_origin(true), view_mat(noob::identity_mat4()), projection_mat(noob::identity_mat4()), basic_lights( { noob::vec4(0.0, 1.0, 0.0, 0.3), noob::vec4(1.0, 0.0, 0.0, 0.3) } ), bodies_mapping(draw_graph), basic_models_mapping(draw_graph), shaders_mapping(draw_graph) {}

			~stage();

			// This one must be called by the application. It really sucks but that's because the graphics API is (currently) static. This may well change soon enough.
			void init();

			// This one provides a way to bring everything back to scratch 
			void tear_down();

			// Call those every frame or so.
			void update(double dt);
			// TODO: Implement
			void draw(float window_width, float window_height) const;

			// Creates physics body. Those get made lots.
			noob::bodies_holder::handle body(const noob::body_type, const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);

			noob::bodies_holder bodies;

			// Functions to create commonly-used configurations:
			void actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const noob::shaders_holder::handle);

			void actor(const noob::bodies_holder::handle, const noob::globals::scaled_model&, const noob::shaders_holder::handle);

			// void actor(const noob::shapes_holder::handle, const noob::vec3& pos, const noob::vec3& orient, const noob::vec3& scale, const noob::shaders_holder::handle);

			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			void scenery(const noob::basic_mesh&, const noob::vec3& pos, const noob::versor& orient, const noob::shaders_holder::handle, const std::string& name);

			bool show_origin;

			noob::mat4 view_mat, projection_mat;

			void set_basic_light(unsigned int i, const noob::vec4& light);

			noob::vec4 get_basic_light(unsigned int i) const;

			noob::prepared_shaders renderer;


		protected:
			const int NUM_RESERVED_NODES = 12000;			
			const int NUM_RESERVED_ARCS = 12000;

			std::array<noob::vec4, 2> basic_lights;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			lemon::ListDigraph draw_graph;
			lemon::ListDigraph::NodeMap<noob::bodies_holder::handle> bodies_mapping;
			lemon::ListDigraph::NodeMap<noob::basic_models_holder::handle> basic_models_mapping;
			lemon::ListDigraph::NodeMap<noob::shaders_holder::handle> shaders_mapping;
			lemon::ListDigraph::Node root_node;

			std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
			std::map<size_t, lemon::ListDigraph::Node> bodies_to_nodes;
			std::map<size_t, lemon::ListDigraph::Node> basic_models_to_nodes;
			std::map<size_t, lemon::ListDigraph::Node> shaders_to_nodes;


	};
}
