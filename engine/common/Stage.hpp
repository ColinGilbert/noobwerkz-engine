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
#include "PhysicsContacts.hpp"

#include "reactphysics3d.h"

namespace noob
{
	class actor;
	class stage
	{
		public:
			stage() : world(rp3d::Vector3(0.0, -9.81, 0.0), rp3d::decimal(1.0 / 60.0)), paused(false) {}

			bool init();
			
			void update(double dt);
			
			void draw() const;
			void draw(noob::prop*) const;
			void draw(const std::shared_ptr<noob::actor>&) const;
			
			void debug_draw(noob::prop*) const;
			void debug_draw(const std::shared_ptr<noob::actor>&) const;
			
			void pause() { paused = true; }
			void start() { paused = false; }

			std::shared_ptr<noob::actor> make_actor(const std::string& name, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::skeletal_anim>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& transform, float mass = 2.0, float width = 0.25, float height = 1.0, float max_speed = 5.0);
			
			std::shared_ptr<noob::prop> make_prop(const std::string& name, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& transform);

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
			
			

			//rp3d::CollisionBody* physics_body(float friction, float bouncy, float linear_damping, float angular_damping, noob::mat4 world_transform = noob::identity_mat4(), body_type type = noob::stage::body_type::DYNAMIC, bool gravity = true);
			


			void draw_pose(const std::shared_ptr<noob::skeletal_anim>&);//, const std::string& anim_name, float time);
			
		protected:
			rp3d::DynamicsWorld world;
			noob::physics_event_listener phyz_listener;
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
			std::forward_list<noob::actor> debug_actors;
			std::unordered_map<std::string, std::shared_ptr<noob::prepared_shaders::info>> shader_uniforms;
			std::unordered_map<std::string, std::shared_ptr<noob::model>> models;
			std::unordered_map<std::string, std::shared_ptr<noob::skeletal_anim>> skeletons;
	};
}

