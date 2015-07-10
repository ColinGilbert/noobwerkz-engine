#pragma once

#include "Mesh.hpp"
#include "NoobUtils.hpp"
#include "MathFuncs.hpp"
#include "PhysicsWorld.hpp"

#include "VoxelWorld.hpp"

// #include "Noise.h"
#include "Materials.hpp"
#include "Actor.hpp"
#include "BehaviourTree.hpp"
#include "HACDRenderer.hpp"
#include "TriPlanar.hpp"

// #include <lemon/list_graph.h>
// #include <lemon/static_graph.h>

namespace noob
{
	class scene
	{
		public:
			struct terrain_info
			{
				noob::actor model;
				noob::triplanar_renderer::uniform_info colouring_info;
			};
			virtual ~scene()
			{
				for (auto a : terrains)
				{
					delete a;
				}
			};
			void init();
			//void add_actor(const noob::actor& a);
			void draw();

			noob::physics_world physics;

			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;
			std::vector<noob::scene::terrain_info*> terrains;


		protected:
			// lemon::ListDigraph dynamic_graph;
			// lemon::StaticDigraph fast_graph;
	};

}
