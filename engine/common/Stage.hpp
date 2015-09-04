#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <boost/variant.hpp>

#include "Drawable.hpp"
#include "Config.hpp"
#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "ShaderVariant.hpp"
#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"
#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "Model.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
#include "Prop.hpp"
#include "Scenery.hpp"

#include <btBulletDynamicsCommon.h>

namespace noob
{
	class prop;
	class actor;
	class scenery;
	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);
			
			void draw() const;
			// TODO: Replace many of these functions with type-safe, templated code
			// Loads a serialized model (from cereal binary)
			size_t add_model(const std::string& filename);
			size_t add_model(const noob::basic_mesh&);
			size_t add_drawable(size_t model, size_t light, size_t reflectance, size_t shading, const noob::vec3& _scale = noob::vec3(1.0, 1.0, 1.0)); 
			size_t add_skeleton(const std::string& filename);
			size_t add_actor(size_t drawable, size_t skeletal_anim, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			size_t add_prop(btRigidBody*, size_t drawable, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			size_t add_scenery(size_t drawable, const noob::vec3&, const noob::versor& = noob::versor(0.0, 0.0, 0.0, 1.0));
			size_t add_light(const noob::light&);
			size_t add_reflectance(const noob::reflectance&);
			size_t add_shader(const noob::prepared_shaders::info&);

			void set_light(size_t, const noob::light&);
			void set_reflectance(size_t, const noob::reflectance&);
			void set_shader(size_t, const noob::prepared_shaders::info&);
		
			// This is done to prevent implicit string-to-int conversions, which is semi-valid C++ (using -fpermissive) but would likely yield unwanted results for the programmer.
			void set_light(const std::string&, const noob::light&) = delete;
			void set_reflectance(const std::string&, const noob::reflectance&) = delete;
			void set_shader(const std::string&, const noob::prepared_shaders::info&) = delete;

			void set_model_name(size_t, const std::string&);
			void set_drawable_name(size_t, const std::string&);
			void set_skeleton_name(size_t, const std::string&);
			void set_actor_name(size_t, const std::string&);
			void set_prop_name(size_t, const std::string&);
			void set_scenery_name(size_t, const std::string&);
			void set_light_name(size_t, const std::string&);
			void set_reflectance_name(size_t, const std::string&);
			void set_shader_name(size_t, const std::string&);

			size_t get_model_id(const std::string&) const;
			size_t get_drawable_id(const std::string&) const;
			size_t get_skeleton_id(const std::string&) const;
			size_t get_actor_id(const std::string&) const;
			size_t get_prop_id(const std::string&) const;
			size_t get_scenery_id(const std::string&) const;
			size_t get_light_id(const std::string&) const;
			size_t get_reflectance_id(const std::string&) const;
			size_t get_shader_id(const std::string&) const;

			noob::model* get_model(size_t) const;
			noob::drawable* get_drawable(size_t) const;
			noob::skeletal_anim* get_skeleton(size_t) const;
			noob::actor* get_actor(size_t) const;
			noob::prop* get_prop(size_t) const;
			noob::scenery* get_scenery(size_t) const;
			const noob::light* get_light(size_t) const;
			const noob::reflectance* get_reflectance(size_t) const;
			const noob::prepared_shaders::info* get_shader(size_t) const;

			bool model_exists(size_t) const;
			bool drawable_exists(size_t) const;
			bool skeleton_exists(size_t) const;
			bool actor_exists(size_t) const;
			bool prop_exists(size_t) const;
			bool scenery_exists(size_t) const;
			bool light_exists(size_t) const;
			bool reflectance_exists(size_t) const;
			bool shader_exists(size_t) const;

			// TODO: Replace with tags(?)
			btRigidBody* body(btCollisionShape*, float mass, const noob::vec3& pos, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			
			// These cache the shape for reuse, as they are simple parametrics.
			// TODO: Since these are globals, make them static, and/or separate from the stage class.
			btSphereShape* sphere(float r);
			btBoxShape* box(float x, float y, float z);
			btCylinderShape* cylinder(float r, float h);
			btConeShape* cone(float r, float h);
			btCapsuleShape* capsule(float r, float h);
			btStaticPlaneShape* plane(const noob::vec3& normal, float offset);
			
			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively. Might provide a way to cache frequently-used ones if needed.
			btConvexHullShape* hull(const std::vector<noob::vec3>& point);
			//btCompoundShape* breakable_mesh(const noob::basic_mesh&);
			//btCompoundShape* breakable_mesh(const std::vector<noob::basic_mesh>&);


			// Public member.
			bool paused;
			
		protected:
			// Protected function(s):
			void draw(noob::drawable*, const noob::mat4&) const;
			
			// Protected members:
			noob::prepared_shaders shaders;


			// TODO: Templatize these.
			std::unordered_map<std::string, size_t> model_names;
			std::vector<std::unique_ptr<noob::model>> models;
			
			std::unordered_map<std::string, size_t> drawable_names;
			std::vector<std::unique_ptr<noob::drawable>> drawables;
			
			std::unordered_map<std::string, size_t> actor_names;
			std::vector<std::unique_ptr<noob::actor>> actors;
			
			std::unordered_map<std::string, size_t> prop_names;
			std::vector<std::unique_ptr<noob::prop>> props;
			
			std::unordered_map<std::string, size_t> scenery_names;
			std::vector<std::unique_ptr<noob::scenery>> sceneries;
			
			std::unordered_map<std::string, size_t> skeleton_names;
			std::vector<std::unique_ptr<noob::skeletal_anim>> skeletons;
			
			std::unordered_map<std::string, size_t> light_names;
			std::vector<noob::light> lights;
			
			std::unordered_map<std::string, size_t> reflectance_names;
			std::vector<noob::reflectance> reflectances;

			std::unordered_map<std::string, size_t> shading_names;
			std::vector<noob::prepared_shaders::info> shader_uniforms;

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			std::unordered_map<float, btSphereShape*> spheres;
			std::map<std::tuple<float, float, float>, btBoxShape*> boxes;
			std::map<std::tuple<float, float>, btCylinderShape*> cylinders;
			std::map<std::tuple<float, float>, btConeShape*> cones;
			std::map<std::tuple<float, float>, btCapsuleShape*> capsules;
			std::map<std::tuple<float,float,float,float>, btStaticPlaneShape*> planes;


	};
}

