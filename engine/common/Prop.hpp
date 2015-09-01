// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once

#include "MathFuncs.hpp"
#include "Model.hpp"
#include "ShaderVariant.hpp"
#include <memory>

#include "reactphysics3d.h"

namespace noob
{
	struct prop
	{
			prop() : body(nullptr), drawing_scale(noob::vec3(1.0, 1.0, 1.0)) {}

			void init(rp3d::RigidBody*, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&);
			noob::mat4 get_transform() const;
			noob::vec3 get_position() const;
			noob::versor get_orientation() const;
			noob::vec3 get_linear_velocity() const;
			noob::vec3 get_angular_velocity() const;
			std::string get_debug_info() const;

			void attach_sphere(float radius, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			void attach_box(float width, float height, float depth, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			void attach_capsule(float radius, float height, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));	
			void attach_cylinder(float radius, float height, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			void attach_cone(float radius, float height, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));
			void attach_hull(const noob::basic_mesh& mesh, float mass, const noob::vec3& position = noob::vec3(0.0, 0.0, 0.0), const noob::versor& orientation = noob::versor(0.0, 0.0, 0.0, 1.0));

			
			rp3d::RigidBody* body;
			std::shared_ptr<noob::prepared_shaders::info> shading;
			std::shared_ptr<noob::model> model;
			noob::vec3 drawing_scale;
	};
}
