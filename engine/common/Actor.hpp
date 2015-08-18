#pragma once


#include <vector>
#include <memory>
#include <deque>
#include "MathFuncs.hpp"
#include "AnimatedModel.hpp"
#include "PhysicsWorld.hpp"
#include "Drawable3D.hpp"
#include "Graphics.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	struct actor
	{
			actor(const std::shared_ptr<noob::animated_model>& _anim, const std::shared_ptr<noob::drawable3d>& _drawable) : anim(_anim), drawable(_drawable), xform(noob::identity_mat4()), name("placeholder"), current_anim("idle") {}

			void update();
			void add_to_path(const std::vector<noob::vec3>& path_segment);
			void clear_path();
			std::vector<noob::vec3> get_path_vector() const;
			void draw() const;


			// Public members
			noob::physics_body body, destination;
			std::weak_ptr<noob::animated_model> anim;
			std::weak_ptr<noob::drawable3d> drawable;
			std::deque<noob::vec3> path;
			noob::mat4 xform;
			noob::shaders::info shader_info;
			std::string name, current_anim;
			float anim_time;
	};
}
