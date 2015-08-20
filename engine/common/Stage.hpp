#pragma once


#include "Config.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"

#include "ShaderVariant.hpp"

#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"

#include <string>
#include <tuple>

#include <es/storage.hpp>
#include <es/entity.hpp>
#include <es/component.hpp>

#include "Actor.hpp"
#include "PhysicsWorld.hpp"
#include "SkeletalAnim.hpp"
#include "Model.hpp"

namespace noob
{
	class stage
	{
		public:
			stage() : paused(false) {}
			
			typedef es::entity actor_id;

			bool init();
			void update(double dt);
			void draw();
			// enum component_type { LIFETIME, TRANSFORM, BODY, SHAPE, MODEL, SHADER, SKELETON; }
			actor_id get_actor(const std::string& name);
			// std::vector<actor_id> get_actors_with_type(noob::stage::component_type);
			
			float get_lifetime(actor_id) const;
			noob::mat4 get_transform(actor_id) const;
			noob::physics_body get_body(actor_id) const;
			noob::physics_shape get_shape(actor_id) const;
			std::weak_ptr<noob::model> get_model(actor_id) const;
			std::weak_ptr<noob::prepared_shaders::info> get_shader(actor_id) const;
			std::weak_ptr<noob::skeletal_anim> get_skeleton(actor_id) const;

			void set_lifetime(actor_id, float);
			void set_transform(actor_id, const noob::mat4&);
			void set_body(actor_id, const noob::physics_body&);
			void set_shape(actor_id, const noob::physics_shape&);
			void set_model(actor_id, const std::weak_ptr<noob::model>&);
			void set_shader(actor_id, const std::weak_ptr<noob::prepared_shaders::info>&);
			void set_skeleton(actor_id, const std::weak_ptr<noob::skeletal_anim>&);

			// Loads a serialized model (from cereal binary)
			void add_model(const std::string& name, const std::string& filename);
			void add_model(const std::string& name, const noob::basic_mesh&);
			void add_skeleton(const std::string& name, const std::string& filename);

			std::weak_ptr<noob::model> get_model(const std::string& name) const;
			std::weak_ptr<noob::skeletal_anim> get_skeleton(const std::string& name) const;

		protected:
			es::storage storage;
			noob::physics_world world;
			noob::prepared_shaders shaders;
			bool paused;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::triplanar_renderer triplanar_render;
			noob::basic_renderer basic_render;

			noob::mat4 view_mat, projection_mat;

			// For fast access to basic shapes. Test to see difference.
			std::weak_ptr<noob::model> unit_cube, unit_sphere, unit_cylinder, unit_cone;
			
			// Actors without lifetime stay forever.
			es::storage::component_id lifetime;
			// For actors that move themselves
			es::storage::component_id transform;
			// For actors driven by physics
			es::storage::component_id body;
			es::storage::component_id shape;
			// Drawing info
			es::storage::component_id model;
			es::storage::component_id shader;
			// Animation
			es::storage::component_id anim_skel;
			es::storage::component_id anim_name;
			es::storage::component_id anim_time;
			// Proper names, for scripting
			es::storage::component_id actor_name;

			
			std::unordered_map<std::string, std::shared_ptr<noob::prepared_shaders::info>> shader_uniforms;
			std::unordered_map<std::string, std::shared_ptr<noob::model>> models;
			std::unordered_map<std::string, std::shared_ptr<noob::skeletal_anim>> skeletons;
	};
}

