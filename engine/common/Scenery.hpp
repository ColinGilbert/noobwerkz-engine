// A piece of scenery is simply a static physics body, its mesh shape, and a model associated.
#pragma once

#include <memory>

#include "Model.hpp"
#include "MathFuncs.hpp"
#include <btBulletDynamicsCommon.h>
#include "ShaderVariant.hpp"
namespace noob
{
	class scenery
	{
		public:
			scenery(btDiscreteDynamicsWorld* _world, const noob::basic_mesh& _mesh, const std::shared_ptr<noob::prepared_shaders::info>& _uniforms, const noob::vec3& _position, const noob::versor& _orientation);			
			void set_shader(const std::shared_ptr<noob::prepared_shaders::info>& _uniforms) { uniforms = _uniforms; }
			noob::prepared_shaders::info* get_shading() const { return uniforms.get(); }
			noob::mat4 get_transform() const { return transform; }
			noob::model* get_model() const { return const_cast<noob::model*>(model.get()); }	
		protected:
			btRigidBody* body;
			btBvhTriangleMeshShape* shape;
			btDiscreteDynamicsWorld* world;

			std::shared_ptr<noob::prepared_shaders::info> uniforms;
			std::unique_ptr<noob::model> model;
			noob::mat4 transform;

	};
}
