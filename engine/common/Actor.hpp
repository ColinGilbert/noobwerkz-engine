#pragma once


#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
//#include "PhysicsWorld.hpp"
#include "CharacterController.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"
#include "reactphysics3d.h"

namespace noob
{
	class actor
	{
		public:
			actor() : anim_time(0.0) {}
			
			void set_drawable(const std::shared_ptr<noob::model>& _drawable); 
			void set_skeleton(const std::shared_ptr<noob::skeletal_anim>& _anim);
			void set_shading(const std::shared_ptr<noob::prepared_shaders::info>& _shader_info);
			void set_controller(rp3d::DynamicsWorld* world, const noob::mat4& transform, float mass, float width, float height, float max_speed);
			
			void update(double dt, bool forward = false, bool backward = false, bool left = false, bool right = false, bool jump = false);
			noob::mat4 get_transform() const { return controller.get_prop().get_transform(); }
			noob::prepared_shaders::info get_shading() const { return *shader_info; }
			noob::model* get_model() const { return drawable.get(); }

			bool set_destination(const noob::vec3&);
			bool add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;

			void print_debug_info() const { controller.print_debug_info(); }
			rp3d::RigidBody* get_body() const { return controller.get_prop().get_body(); }

		protected:
			//rp3d::RigidBody* body;
			//rp3d::CollisionBody* destination;
			bool model_valid, skel_valid, shader_valid;
			//noob::physics_body destination;
			noob::character_controller controller;
			std::shared_ptr<noob::skeletal_anim> anim;
			std::shared_ptr<noob::model> drawable;
			std::shared_ptr<noob::prepared_shaders::info> shader_info;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time;
	};
}
