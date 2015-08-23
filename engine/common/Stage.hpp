#pragma once

#include <stack>
#include <string>
#include <tuple>

#include "Config.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"

#include "ShaderVariant.hpp"

#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"


/*
#include <es/entity.hpp>
#include <es/component.hpp>
#include <es/storage.hpp>
*/

#include "Actor.hpp"
#include "PhysicsWorld.hpp"
#include "SkeletalAnim.hpp"
#include "Model.hpp"

namespace noob
{
	class stage
	{
		public:
		//	stage() : actor_counter(0) {}
		/*	
			struct actor_handle
			{
				actor_handle() : valid(false), id(0) {}
				bool valid;
				size_t id;
			};
		*/	
			bool init();
			void update(double dt);
			void draw();
			
			std::shared_ptr<noob::actor> make_actor(const std::string& name, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::skeletal_anim>&, const std::shared_ptr<noob::prepared_shaders::info>&, const std::shared_ptr<noob::physics_shape>& shape, const noob::mat4& transform = noob::identity_mat4(), float mass = 1.0, float max_speed = 5.0, float step_height = 0.25);

			// Loads a serialized model (from cereal binary)
			void add_model(const std::string& name, const std::string& filename);
			void add_model(const std::string& name, const noob::basic_mesh&);
			void add_skeleton(const std::string& name, const std::string& filename);

			std::weak_ptr<noob::actor> get_actor(const std::string& name) const;
			std::weak_ptr<noob::prepared_shaders::info> get_shader(const std::string& name) const;
			std::weak_ptr<noob::model> get_model(const std::string& name) const;
			std::weak_ptr<noob::skeletal_anim> get_skeleton(const std::string& name) const;

		protected:
			noob::physics_world world;
			noob::prepared_shaders shaders;
			bool paused;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::triplanar_renderer triplanar_render;
			noob::basic_renderer basic_render;
			noob::mat4 view_mat, projection_mat;

			// For fast access to basic shapes. Test to see difference.
			std::shared_ptr<noob::model> unit_cube, unit_sphere, unit_cylinder, unit_cone;

			std::unordered_map<std::string, std::shared_ptr<noob::actor>> actors;
			std::unordered_map<std::string, std::shared_ptr<noob::prepared_shaders::info>> shader_uniforms;
			std::unordered_map<std::string, std::shared_ptr<noob::model>> models;
			std::unordered_map<std::string, std::shared_ptr<noob::skeletal_anim>> skeletons;
	};
}

