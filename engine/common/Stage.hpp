#pragma once


#include "Config.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
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
			void init();
			void update(double dt);
			void draw();

			// Loads a serialized model
			// void add_drawable(const std::string& name, const std::string& filename);
			// void add_drawable(const std::string& name, const noob::basic_mesh&);
			// noob::drawable3d* get_drawable(const std::string& name) const;
			
			void add_skeleton(const std::string& name, const std::string& filename); 
			noob::skeletal_anim* get_skeleton(const std::string& name) const;

		protected:
			es::storage storage;
			noob::physics_world world;
			
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::triplanar_renderer triplanar_render;
			noob::basic_renderer basic_render;

			noob::mat4 view_mat, projection_mat;
		
			es::storage::component_id transform;
			es::storage::component_id body;
			es::storage::component_id shape;
			es::storage::component_id model;
			es::storage::component_id triplanar_shader;
			es::storage::component_id basic_shader;
			es::storage::component_id anim_name;
			es::storage::component_id anim_time;
			es::storage::component_id name;

			noob::model* unit_cube;
			noob::model* unit_sphere;
			noob::model* unit_cylinder;
			noob::model* unit_cone;


			std::unordered_map<std::string, std::unique_ptr<noob::model>> models;
			std::unordered_map<std::string, std::unique_ptr<noob::skeletal_anim>> skeletons;
	};
}

