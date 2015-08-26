// A prop is a body with a drawable attached to it. It's basically a "dumb" item with a drawable associated with it.

#pragma once

#include "MathFuncs.hpp"
#include "reactphysics3d.h"
#include "Model.hpp"
#include "ShaderVariant.hpp"

namespace noob
{
	class prop
	{
		public:
			enum type { DYNAMIC, KINEMATIC, STATIC, GHOST };

			void init(rp3d::DynamicsWorld*, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& = noob::identity_mat4(), noob::prop::type = noob::prop::type::DYNAMIC);
			
			void set_transform(const noob::mat4& transform);
			noob::mat4 get_transform() const;

			void set_shading(const std::shared_ptr<noob::prepared_shaders::info>& uniforms) { shader_info = uniforms; }
			std::weak_ptr<noob::prepared_shaders::info> get_shading() const { return shader_info; }
			
			void set_model(const std::shared_ptr<noob::model>& m) { model = m; }
			std::weak_ptr<noob::model> get_model() const { return model; }

			void print_debug_info() const;
			rp3d::RigidBody* get_body() const { return body; }
		protected:
			rp3d::RigidBody* body;
			rp3d::DynamicsWorld* world;
			std::shared_ptr<noob::prepared_shaders::info> shader_info;
			std::shared_ptr<noob::model> model;
	};
}
