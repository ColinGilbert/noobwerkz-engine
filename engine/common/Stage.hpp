#pragma once


#include "Config.hpp"
#include "Drawable3D.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "HACDRenderer.hpp"
#include "ShaderVariant.hpp"
#include "TransformHelper.hpp"

#include <string>
#include <tuple>

#include "Actor.hpp"
#include "PhysicsWorld.hpp"
#include "AnimatedModel.hpp"

namespace noob
{
	class stage
	{
		public:
			void init();
			void update(double dt);
			void draw();

			void add_drawable(const std::string& name, const noob::mesh&);
			std::shared_ptr<noob::drawable3d> get_drawable(const std::string& name) const;
			void remove_actor(const std::string& name);

			void add_actor(const std::string& name, const noob::actor& actor);
			noob::actor get_actor(const std::string& name) const;

			void add_skeleton(const std::string& name, const std::string& filename); 
			std::weak_ptr<noob::animated_model> get_skeleton(const std::string& name) const;


			
			void set_shader(const std::string& name, const noob::shaders::info& info);
			void set_drawing_technique(const std::string& actor_name, const std::string& shader_name);
			void face_point(const noob::actor& a, const noob::vec3& point);

			void draw(const noob::actor& a) const;
			void draw_skeleton(const noob::actor& a) const;


		protected:
			noob::physics_world world;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::hacd_renderer hacd_render;	
			noob::shaders shaders;
			
			noob::mat4 view_mat;
			noob::mat4 projection_mat;
			
			std::shared_ptr<noob::drawable3d> unit_cube, unit_sphere, unit_cylinder, unit_cone, unit_square, unit_triangle;
			
			std::unordered_map<std::string, noob::shaders::info> uniforms;
			std::unordered_map<std::string, noob::actor> actors;
			std::unordered_map<std::string, std::shared_ptr<noob::drawable3d>> drawables;
			std::unordered_map<std::string, std::shared_ptr<noob::animated_model>> skeletons;
	};
}

