#pragma once

#include <lemon/list_graph.h>
#include <lemon/static_graph.h>

namespace noob
{
	class behaviour_tree
	{
		public:
			enum behaviours
			{
				TACTICAL_MOVE, STRATEGIC_MOVE, PATROL, ATTACK, STAND_GROUND, HIDE, FLEE
			};

			void init();
			
		protected:
			lemon::StaticDigraph graph;
	};

}
