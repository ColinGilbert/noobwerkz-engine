// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once

#include "MathFuncs.hpp"
#include "reactphysics3d.h"
#include "Model.hpp"
#include "ShaderVariant.hpp"
#include <memory>
namespace noob
{
	class prop
	{
		public:
			enum type { DYNAMIC, KINEMATIC, STATIC, GHOST };

			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& = noob::identity_mat4(), rp3d::BodyType = rp3d::DYNAMIC);
			
			void set_transform(const noob::mat4& transform);
			noob::mat4 get_transform() const;

			void set_shading(const std::shared_ptr<noob::prepared_shaders::info>& uniforms) { shader_info = uniforms; }
			std::shared_ptr<noob::prepared_shaders::info> get_shading() const { return shader_info; }
			
			void set_model(const std::shared_ptr<noob::model>& m) { model = m; }
			std::shared_ptr<noob::model> get_model() const { return model; }

			void print_debug_info() const;
			rp3d::RigidBody* get_body() const { return body; }


			void add_sphere(float radius, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_box(float width, float height, float depth, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cylinder(float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_capsule(float radius, float height,  float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cone(float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			// This one creates a mesh from several convex hulls (ideally created beforehand via the noob::basic_mesh::convex_decomposition() interface.) May bugger up if meshes aren't convex
			void add_mesh(const std::vector<noob::basic_mesh>& mesh, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());

		protected:
			rp3d::RigidBody* body;
			rp3d::DynamicsWorld* world;
			std::shared_ptr<noob::prepared_shaders::info> shader_info;
			std::shared_ptr<noob::model> model;
	};
}
