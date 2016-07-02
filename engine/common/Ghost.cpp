#include "Ghost.hpp"

#include "Globals.hpp"

void noob::ghost::init(const noob::shape& shape_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg)
{
	inner->setCollisionShape(shape_arg.inner_shape);
	btTransform trans(btQuaternion(orient_arg.q[0], orient_arg.q[1], orient_arg.q[2], orient_arg.q[3]), btVector3(pos_arg.v[0], pos_arg.v[1], pos_arg.v[2]));
	inner->setWorldTransform(trans);
	noob::globals& g = noob::globals::get_instance();
	inner->setUserPointer(reinterpret_cast<void*>(&g.ghost_body_descriptor));
	inner->setUserIndex(-1);
}


void noob::ghost::set_position(const noob::vec3& pos)
{
	btTransform xform(inner->getWorldTransform());
	inner->setWorldTransform(btTransform(xform.getRotation(), btVector3(pos.v[0], pos.v[1], pos.v[2])));
}


void noob::ghost::set_orientation(const noob::versor& orient)
{
	btTransform xform(inner->getWorldTransform());
	inner->setWorldTransform(btTransform(btQuaternion(orient.q[0], orient.q[1], orient.q[2], orient.q[3]), xform.getOrigin()));
}


noob::vec3 noob::ghost::get_position() const
{
	btTransform xform(inner->getWorldTransform());
	return noob::vec3_from_bullet(xform.getOrigin());
}


noob::versor noob::ghost::get_orientation() const
{
	btTransform xform(inner->getWorldTransform());
	return xform.getRotation();
}


noob::mat4 noob::ghost::get_transform() const
{
	return inner->getWorldTransform();
}
