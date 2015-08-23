#pragma once


#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "PhysicsWorld.hpp"
#include "CharacterController.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	class actor
	{
		public:
			actor() : xform(noob::identity_mat4()), anim_time(0.0) {}

			void set_drawable(const std::shared_ptr<noob::model>& _drawable); 
			void set_skeleton(const std::shared_ptr<noob::skeletal_anim>& _anim);
			void set_shading(const std::shared_ptr<noob::prepared_shaders::info>& _shader_info);
			void set_controller(const std::shared_ptr<noob::physics_shape>& shape, const noob::mat4& transform, float mass, float max_speed, float step_height);

			void draw() const;
			void draw_skeleton() const;

			void update(double);

			bool set_destination(const noob::vec3&);
			bool add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;

			// const noob::physics_body* get_body() const { return &body; }

		protected:
		
			bool model_valid, skel_valid, shader_valid;
			noob::mat4 xform;
			noob::physics_body destination;
			noob::character_controller controller;
			std::shared_ptr<noob::skeletal_anim> anim;
			std::shared_ptr<noob::model> drawable;
			std::shared_ptr<noob::prepared_shaders::info> shader_info;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time;
	};
}
