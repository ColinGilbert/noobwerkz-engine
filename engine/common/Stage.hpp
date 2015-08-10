#pragma once

#include "Config.hpp"
#include "Drawable3D.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "HACDRenderer.hpp"
#include "TriplanarGradientMap.hpp"
#include "TransformHelper.hpp"

#include <string>

// #include <es/entity.hpp>
// #include <es/component.hpp>
// #include <es/storage.hpp>

#include "Scenery.hpp"
#include "Actor.hpp"
#include "Prop.hpp"

#include "PhysicsWorld.hpp"
#include "AnimatedModel.hpp"

namespace noob
{
	class stage
	{
		public:
			void init();
			void update(double dt);
			// TODO: Implement
			void draw();

			// std::array<noob::vec4, 6> extract_planes(const noob::mat4& mvp);
			void add_drawable(const std::string& name, const noob::drawable3d* drawable);
			void add_actor(const std::string& name, const noob::actor& actor);
			
		protected:
			noob::physics_world world;
			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;

			noob::mat4 view_mat;
			noob::mat4 projection_mat;
			std::unordered_map<std::string, std::shared_ptr<noob::drawable3d>> drawables;
			std::unordered_map<std::string, std::shared_ptr<noob::animated_model>> animations;
			std::vector<noob::scenery> sceneries;
			std::vector<noob::prop> props;
			std::unordered_map<std::string, noob::actor> actors;
			
			// es::storage world;
	};
}

