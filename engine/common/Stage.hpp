#pragma once

#include <stack>
#include <string>
#include <tuple>
#include <list>
#include <forward_list>
#include <boost/variant.hpp>

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
	struct prop;
	class actor;
	class stage
	{
		public:
			bool init();
			void tear_down();
			void update(double dt);
			
			void draw() const;
			void draw(const std::shared_ptr<noob::scenery>& s) const;
			void draw(noob::prop*) const;
			void draw(const std::shared_ptr<noob::actor>&) const;
			
			//void debug_draw(noob::prop*) const;
			void debug_draw(const std::shared_ptr<noob::actor>&) const;
			
			void pause() { paused = true; }
			void start() { paused = false; }

			std::shared_ptr<noob::actor> make_actor(const std::string& name, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const std::shared_ptr<noob::skeletal_anim>&);
			std::shared_ptr<noob::prop> make_prop(const std::string& name, btRigidBody*, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&);
			std::shared_ptr<noob::scenery> make_scenery(const std::string& _name, const noob::basic_mesh&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::vec3&, const noob::versor&);

			// Loads a serialized model (from cereal binary)
			bool add_model(const std::string& name, const std::string& filename);
			bool add_model(const std::string& name, const noob::basic_mesh&);
			bool add_skeleton(const std::string& name, const std::string& filename);
			void set_shader(const std::string& name, const noob::prepared_shaders::info& info);

			std::weak_ptr<noob::actor> get_actor(const std::string& name) const;
			std::weak_ptr<noob::prop> get_prop(const std::string& name) const;
			std::weak_ptr<noob::prepared_shaders::info> get_shader(const std::string& name) const;
			std::weak_ptr<noob::model> get_model(const std::string& name) const;
			std::weak_ptr<noob::skeletal_anim> get_skeleton(const std::string& name) const;
		
			std::shared_ptr<noob::model> get_unit_cube() const { return unit_cube; }
			std::shared_ptr<noob::model> get_unit_sphere() const { return unit_sphere; }
			std::shared_ptr<noob::model> get_unit_cylinder() const { return unit_cylinder; }
			std::shared_ptr<noob::model> get_unit_cone() const { return unit_cone; }
			
			std::shared_ptr<noob::prepared_shaders::info> get_debug_shader() const { return debug_shader; }

			btRigidBody* body(btCollisionShape*,float mass, const noob::vec3& pos, const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			
			// These cache the shape for reuse, as they are simple parametrics
			btSphereShape* sphere(float r);
			btBoxShape* box(float x, float y, float z);
			btCylinderShape* cylinder(float r, float h);
			btConeShape* cone(float r, float h);
			btCapsuleShape* capsule(float r, float h);
			btStaticPlaneShape* plane(const noob::vec3& normal, float offset);
			
			// These don't cache the shape for reuse, as they are rather difficult to index inexpensively.
			btConvexHullShape* hull(const std::vector<noob::vec3>& point);
			btCompoundShape* breakable_mesh(const noob::basic_mesh&);
			btCompoundShape* breakable_mesh(const std::vector<noob::basic_mesh>&);

			//btBvhTriangleMeshShape* static_mesh(const noob::basic_mesh&);
			//void draw_pose(const std::shared_ptr<noob::skeletal_anim>&);//, const std::string& anim_name, float time);
			
		protected:

			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btSequentialImpulseConstraintSolver* solver;
			btDiscreteDynamicsWorld* dynamics_world;

			noob::prepared_shaders shaders;
			bool paused;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::triplanar_renderer triplanar_render;
			noob::basic_renderer basic_render;
			noob::mat4 view_mat, projection_mat;

			// For fast access to basic shapes. Test to see difference.
			std::shared_ptr<noob::model> unit_cube, unit_sphere, unit_cylinder, unit_cone;
			std::shared_ptr<noob::prepared_shaders::info> debug_shader;
			
			std::unordered_map<std::string, std::shared_ptr<noob::prop>> props;
			std::unordered_map<std::string, std::shared_ptr<noob::actor>> actors;
			std::unordered_map<std::string, std::shared_ptr<noob::scenery>> sceneries;
			std::forward_list<noob::actor> debug_actors;
			std::unordered_map<std::string, std::shared_ptr<noob::prepared_shaders::info>> shader_uniforms;
			std::unordered_map<std::string, std::shared_ptr<noob::model>> models;
			std::unordered_map<std::string, std::shared_ptr<noob::skeletal_anim>> skeletons;

			std::unordered_map<float, btSphereShape*> spheres;
			std::map<std::tuple<float, float, float>, btBoxShape*> boxes;
			std::map<std::tuple<float, float>, btCylinderShape*> cylinders;
			std::map<std::tuple<float, float>, btConeShape*> cones;
			std::map<std::tuple<float, float>, btCapsuleShape*> capsules;
			std::map<std::tuple<float,float,float,float>, btStaticPlaneShape*> planes;
	};
}

