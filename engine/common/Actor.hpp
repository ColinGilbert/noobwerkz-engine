/*
#pragma once


#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "SkeletalAnim.hpp"
#include "PhysicsWorld.hpp"
#include "Drawable3D.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	struct actor
	{
			actor(const std::weak_ptr<noob::skeletal_anim>& _anim, const std::weak_ptr<noob::drawable3d>& _drawable, const std::weak_ptr<noob::shaders::info> _shader_info) : valid(false), anim_raw(nullptr), drawable_raw(nullptr), shader_info_raw(nullptr), xform(noob::identity_mat4()), anim(_anim), drawable(_drawable), shader_info(_shader_info), current_anim("idle"), anim_time(0.0) {}

			void validate();
			void set_drawable(const std::weak_ptr<noob::drawable3d>& _drawable); 
			void set_skeleton(const std::weak_ptr<noob::skeletal_anim>& _anim);
			void set_shader_info(const std::weak_ptr<noob::shaders::info>& _shader_info); 
			void update();
			void add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;
			// void draw() const;
			bool is_valid() const { return valid; }
			//std::string get_name() const { return name; }

			protected:
			void invalidate();

			bool valid;
			noob::skeletal_anim* anim_raw;
			noob::drawable3d* drawable_raw;
			noob::shaders::info* shader_info_raw;
			noob::mat4 xform;
			noob::physics_body body, destination;
			std::weak_ptr<noob::skeletal_anim> anim;
			std::weak_ptr<noob::drawable3d> drawable;
			std::weak_ptr<noob::shaders::info> shader_info;
			std::deque<noob::vec3> path;
			std::string current_anim;
			float anim_time;
	};
}
*/
