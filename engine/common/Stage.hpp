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
			stage() : show_origin(true), view_mat(noob::identity_mat4()), projection_mat(noob::identity_mat4()), basic_lights( { noob::vec4(0.0, 1.0, 0.0, 0.1), noob::vec4(1.0, 0.0, 0.0, 0.1) } ), bodies_mapping(draw_graph), models_mapping(draw_graph), shaders_mapping(draw_graph) {}

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
			noob::bodies_holder::handle body(noob::body_type, noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);

			noob::bodies_holder bodies;

			// Functions to create commonly-used configurations:
			void actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const std::string& shading);

			void prop(const noob::bodies_holder::handle, const noob::basic_models_holder::handle, const std::string& shading);

			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			void scenery(const noob::meshes_holder::handle, const noob::vec3& pos, const std::string& shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));

			bool show_origin;

			noob::mat4 view_mat, projection_mat;

			void set_basic_light(unsigned int i, const noob::vec4& light);

			noob::vec4 get_basic_light(unsigned int i) const;

			noob::prepared_shaders renderer;


		protected:
			std::array<noob::vec4, 2> basic_lights;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			lemon::SmartDigraph draw_graph;
			lemon::SmartDigraph::NodeMap<noob::bodies_holder::handle> bodies_mapping;
			lemon::SmartDigraph::NodeMap<noob::basic_models_holder::handle> models_mapping;
			lemon::SmartDigraph::NodeMap<noob::shaders_holder::handle> shaders_mapping;
			lemon::SmartDigraph::Node root_node;

			std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
	};
}
