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

namespace noob
{
	typedef noob::component<std::unique_ptr<noob::body>> bodies_holder;


	class stage;

	class component_tag
	{
		friend class stage;

		public:
		es::storage::component_id get() const { return inner; }
		private:
		es::storage::component_id inner;
	};

	class stage
	{
		public:
			stage() : show_origin(true), view_mat(noob::identity_mat4()), projection_mat(noob::identity_mat4()), basic_lights( { noob::vec4(0.0, 1.0, 0.0, 0.1), noob::vec4(1.0, 0.0, 0.0, 0.1) } ) {}
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
			unsigned int _body(noob::body_type, unsigned int, float, const noob::vec3&, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);

			// Tags that are used for faster access to our tag-tracker
			noob::component_tag scales_tag, mesh_tag, path_tag, shape_tag, shape_type_tag, body_tag, basic_model_tag, animated_model_tag, skeletal_anim_tag, shader_tag;

			noob::bodies_holder bodies;

			// Functions to create commonly-used configurations:
			// Actors are a ghost body with a skeletal model. They are controlled by custom logic in order to cut down on physics engine time
			es::entity actor(const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const std::string& shading);
			unsigned int _actor(unsigned int _bod, unsigned int _model, const std::string& shading);

			// Props are simple rigid-body objects with leaned-down 3d models that cannot be animated via vertex weights. They also cannot apply movement to themselves.
			// Note: Bullet doesn't support movable trimeshes, so trimeshes passed into this function get implicitly turned into scenery.
			es::entity prop(const noob::bodies_holder::handle, const std::string& shading);
			unsigned int _prop(unsigned int _bod, const std::string& shading);

			//es::entity prop(const noob::bodies_holder::handle, const std::string& shading);
			es::entity prop(const noob::bodies_holder::handle, const noob::basic_models_holder::handle, const std::string& shading);
			unsigned int _prop(unsigned int _bod, unsigned int _model, const std::string& shading);

			// Scenery is a non-movable item that uses indexed triangle meshes as input.
			es::entity scenery(const noob::meshes_holder::handle, const noob::vec3& pos, const std::string& shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));
			unsigned int _scenery(unsigned int _mesh, const noob::vec3& pos, const std::string& _shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0));

			es::storage pool;

			bool show_origin;

			noob::mat4 view_mat, projection_mat;

			void set_basic_light(unsigned int i, const noob::vec4& light);
			noob::vec4 get_basic_light(unsigned int i) const;

			noob::prepared_shaders renderer;
		protected:
			// template<typename T>
			// unsigned char register_es_component(T t, const std::string& friendly_name)
			// {
			// auto a (pool.register_component<T>(t, friendly_name));
			// return a;
			// }

			// Our component-entity system: The super-efficient handle-swapping fast-iterating dynamic magic enabler
			// Shader setting	

			std::array<noob::vec4, 2> basic_lights;

			// noob::prepared_shaders renderer;

			// dcollide::World ghost_world;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;
			
			


			// std::map<float, basic_models_holder::handle> sphere_models;
			// std::map<std::tuple<float, float, float>, basic_models_holder::handle> box_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> cylinder_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> cone_models;
			// std::map<std::tuple<float, float>, basic_models_holder::handle> capsule_models;
			// std::map<std::tuple<float,float,float,float>, basic_models_holder::handle> planes;
			// std::map<std::vector<std::array<float,3>>, basic_models_holder::handle> hull_models;
			// std::unordered_map<std::string, basic_models_holder::handle> trimesh_models;

			std::unordered_map<size_t, noob::shapes_holder::handle> bodies_to_shapes;
	};
}
