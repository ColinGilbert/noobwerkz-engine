// THis class refers to a volumetric mesh that leverages the power of Lemon Graph, Voro++, and Bullet.
// Uses convex shapes connected to other convex shapes by at least one face to enforce an enclosed volume

#pragma once

#include <voro++.hh>
#include <lemon/smart_graph.h>

#include "Ghost.hpp"
#include "Shape.hpp"
#include "Globals.hpp"
#include "ComponentDefines.hpp"

namespace noob
{
	class active_mesh_ng
	{
		public:
			void init();


		protected:
			btBroadphaseInterface* broadphase;
			btDefaultCollisionConfiguration* collision_configuration;
			btCollisionDispatcher* collision_dispatcher;
			btCollisionWorld* collision_world;	
			
			std::vector<noob::ghost> ghosts;
			std::vector<noob::shape> shapes;
	};
}
