// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once

#include "MathFuncs.hpp"
#include "reactphysics3d.h"
#include "Model.hpp"
#include "ShaderVariant.hpp"
#include <memory>

namespace noob
{
	struct prop
	{
		//	prop() : model_valid(false), shader_valid(false), body(nullptr), world(nullptr) {}

			struct info
			{
				noob::vec3 position;
				noob::versor orientation;
			};

			enum type { DYNAMIC, KINEMATIC, STATIC, GHOST };

			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::model>& _actor_model, const std::shared_ptr<noob::prepared_shaders::info>& _actor_shading, const noob::mat4& = noob::identity_mat4(), rp3d::BodyType = rp3d::DYNAMIC);
			void set_transform(const noob::mat4& transform);
			noob::mat4 get_transform() const;

			void print_debug_info() const;
			noob::prop::info get_info() const;

			void add_sphere(float radius, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_box(float width, float height, float depth, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cylinder(float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_capsule(float radius, float height,  float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cone(float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			// This one creates a mesh from several convex hulls (ideally created beforehand via the noob::basic_mesh::convex_decomposition() interface.) May bugger up if meshes aren't convex
			void add_mesh(const std::vector<noob::basic_mesh>& mesh, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
		
			rp3d::RigidBody* body;
			rp3d::DynamicsWorld* world;
			std::shared_ptr<noob::prepared_shaders::info> shading;
			std::shared_ptr<noob::model> model;

	};
}
