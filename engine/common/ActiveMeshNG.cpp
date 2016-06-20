#include "ActiveMeshNG.hpp"

void noob::active_mesh_ng::init()
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	collision_world = new btCollisionWorld(collision_dispatcher, broadphase, collision_configuration); 
}
