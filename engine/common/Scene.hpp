#pragma once

#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"
#include "Actor.hpp"
#include "HACDRenderer.hpp"
#include "TriPlanar.hpp"

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

namespace noob
{
	class scene
	{
		public:
			void init();
			void draw();

			noob::hacd_renderer hacd_render;
			noob::triplanar_renderer triplanar_render;
			
		protected:
			lemon::StaticDigraph fast_graph;
	};

}
