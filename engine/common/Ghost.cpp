#include "Ghost.hpp"

#include "Globals.hpp"

void noob::ghost::init(btCollisionWorld* world, const noob::shape& shape_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg) noexcept(true) 
{
	inner = new btPairCachingGhostObject();
	//logger::log("[Ghost] about to add collision object to world");
	inner->setCollisionShape(shape_arg.inner_shape);
	btTransform trans(btQuaternion(orient_arg.q[0], orient_arg.q[1], orient_arg.q[2], orient_arg.q[3]), btVector3(pos_arg.v[0], pos_arg.v[1], pos_arg.v[2]));
	//logger::log("[Ghost] about to set world transform");
	inner->setWorldTransform(trans);
	world->addCollisionObject(inner);
	// noob::globals& g = noob::globals::get_instance();
	// inner->setUserPointer(static_cast<void*>(&g.ghost_body_descriptor));
	// inner->setUserIndex(std::numeric_limits<uint32_t>::max());
}


void noob::ghost::set_position(const noob::vec3& pos) noexcept(true) 
{
	btTransform xform(inner->getWorldTransform());
	inner->setWorldTransform(btTransform(xform.getRotation(), btVector3(pos.v[0], pos.v[1], pos.v[2])));
}


void noob::ghost::set_orientation(const noob::versor& orient) noexcept(true) 
{
	btTransform xform(inner->getWorldTransform());
	inner->setWorldTransform(btTransform(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]), xform.getOrigin()));
}


noob::vec3 noob::ghost::get_position() const noexcept(true) 
{
	btTransform xform(inner->getWorldTransform());
	return noob::vec3_from_bullet(xform.getOrigin());
}


noob::versor noob::ghost::get_orientation() const noexcept(true) 
{
	btTransform xform(inner->getWorldTransform());
	return noob::versor_from_bullet(xform.getRotation());
}


noob::mat4 noob::ghost::get_transform() const noexcept(true) 
{
	return mat4_from_bullet(inner->getWorldTransform());
}

