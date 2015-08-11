
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

#include "Scenery.hpp"
#include "Prop.hpp"
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

			// std::array<noob::vec4, 6> extract_planes(const noob::mat4& mvp);
			void add_drawable(const std::string& name, const noob::drawable3d* drawable);
			void add_actor(const std::string& name, const noob::actor& actor);
			void set_shader(const std::string& name, const noob::shaders::info& info);
			void set_shader(const std::string& actor_name, const std::string& shader_name);
		protected:
			noob::physics_world world;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::hacd_renderer hacd_render;
			
			noob::shaders shaders;
			
			noob::mat4 view_mat;
			noob::mat4 projection_mat;

			std::unordered_map<std::string, noob::shaders::info> uniforms;
			std::unordered_map<std::string, noob::actor> actors;
			std::unordered_map<std::string, std::shared_ptr<noob::drawable3d>> drawables;
			std::unordered_map<std::string, std::shared_ptr<noob::animated_model>> animations;
			std::vector<noob::scenery> sceneries;
			std::vector<noob::prop> props;
			
			// es::storage world;
	};
}

