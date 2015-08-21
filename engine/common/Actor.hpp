#pragma once


#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "PhysicsWorld.hpp"
#include "Model.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	class actor
	{
		public:
		//	actor(const std::weak_ptr<noob::skeletal_anim>& _anim, const std::weak_ptr<noob::model>& _drawable, const std::weak_ptr<noob::prepared_shaders::info> _shader_info) : valid(false), xform(noob::identity_mat4()), anim(_anim), drawable(_drawable), shader_info(_shader_info), current_anim("idle"), anim_time(0.0) {}
			actor() : valid(false), xform(noob::identity_mat4()), anim_time(0.0) {}
			void set_drawable(const std::weak_ptr<noob::model>& _drawable); 
			void set_skeleton(const std::weak_ptr<noob::skeletal_anim>& _anim);
			void set_shading(const std::weak_ptr<noob::prepared_shaders::info>& _shader_info);
			void update();
			void add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;
			void draw() const;
			void draw_skeleton() const;

			protected:
			bool valid;
			noob::mat4 xform;
			noob::physics_body body, destination;
			std::shared_ptr<noob::skeletal_anim> anim;
			std::shared_ptr<noob::model> drawable;
			std::shared_ptr<noob::prepared_shaders::info> shader_info;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time;
	};
}
