// TODO: Implement all creation functions (*) and ensure that they take constructor args
#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>

#include <boost/variant.hpp>
//#include <boost/intrusive_ptr.hpp>

#include <btBulletDynamicsCommon.h>
#include "es/storage.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "ShaderVariant.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "BasicModel.hpp"
#include "AnimatedModel.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"
#include "Body.hpp"
#include "Shape.hpp"

namespace noob
{
	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);

			void draw() const;
			
			// std::shared_ptr<noob::basic_model> basic_model(const noob::basic_mesh&);	
			// std::shared_ptr<noob::basic_model> basic_model(const std::shared_ptr<noob::shape>&);
			
			// Loads a serialized model (from cereal binary)
			// std::shared_ptr<noob::animated_model> animated_model(const std::string& filename, const std::string& friendly_name);
			// std::shared_ptr<noob::skeletal_anim> skeleton(const std::string& filename, const std::string& friendly_name);

			// noob::actor actor(const noob::prop&, const std::shared_ptr<noob::animated_model>&, const std::string& friendly_name);
			// noob::prop prop(const noob::body&, const std::shared_ptr<noob::basic_model>&, const std::string& friendly_name);
			// noob::scenery scenery(const std::shared_ptr<noob::basic_model>&, const noob::vec3& pos, const noob::versor& orient, const std::string& friendly_name);
			// noob::body body(const std::shared_ptr<noob::shape>&, float mass, const noob::vec3& pos, const noob::versor& orient);
			
			// std::shared_ptr<noob::light> light(const std::shared_ptr<noob::light>&);
			// std::shared_ptr<noob::reflection> reflection(const noob::reflection&);
			// std::shared_ptr<noob::shader> shader(const noob::prepared_shaders::info&, const std::string& name);
			
			// std::shared_ptr<noob::shape> sphere(float r);
			// std::shared_ptr<noob::shape> box(float x, float y, float z);
			// std::shared_ptr<noob::shape> cylinder(float r, float h);
			// std::shared_ptr<noob::shape> cone(float r, float h);
			// std::shared_ptr<noob::shape> capsule(float r, float h);
			// std::shared_ptr<noob::shape> plane(const noob::vec3& normal, float offset);
			// std::shared_ptr<noob::shape> hull(const std::vector<noob::vec3>& points, const std::string& name);
			// std::shared_ptr<noob::shape> trimesh(const noob::basic_mesh& mesh, const std::string& name);

			es::storage pool;

		protected:

			noob::prepared_shaders renderer;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			// TODO: Test other data structures.
			// std::map<float, std::shared_ptr<noob::shape>> spheres;
			// std::map<std::tuple<float, float, float>, std::shared_ptr<noob::shape>> boxes;
			// std::map<std::tuple<float, float>, std::shared_ptr<noob::shape>> cylinders;
			// std::map<std::tuple<float, float>, std::shared_ptr<noob::shape>> cones;
			// std::map<std::tuple<float, float>, std::shared_ptr<noob::shape>> capsules;
			// std::map<std::tuple<float,float,float,float>, std::shared_ptr<noob::shape>> planes;
			// std::unordered_map<std::string, std::shared_ptr<noob::shape>> hulls;
			// std::unordered_map<std::string, std::shared_ptr<noob::shape>> trimeshes;
			unsigned char shape, body, basic_model, animated_model, skeletal_anim, basic_shader, triplanar_shader, sphere, box;



	};
}
