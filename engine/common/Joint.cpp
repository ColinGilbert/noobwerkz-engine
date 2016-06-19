#include "Joint.hpp"

void noob::joint::init(const btDynamicsWorld* w, const noob::body& a, const noob::body& b, const noob::mat4& local_a, const noob::mat4& local_b)
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner = new btGeneric6DofSpringConstraint(*(a.inner), *(b.inner), tr_a, tr_b, true);
	inner->setEquilibriumPoint();//0, 0);
	w->addConstraint(inner, true);
}

void noob::joint::set_spring(uint8_t i, bool val)
{
	inner->enableSpring (i, val);
}

void noob::joint::set_stiffness(uint8_t i, float val)
{
	inner->setStiffness(i, val);
}

void noob::joint::set_damping(uint8_t i, float val)
{
	inner->setDamping(i, val);
}

void noob::joint::set_limits(uint8_t i, const noob::vec2& lims)
{
	inner->setLimit(i, lims.v[0], lims.v[1]);
}

void noob::joint::set_frames(const noob::mat4& local_a, const noob::mat4& local_b)
{
	btTransform tr_a;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	btTransform tr_b;
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner->setFrames(tr_a, tr_b);
}

noob::vec3 noob::joint::get_axis(uint8_t i) const
{
	return inner->getAxis(i);
}

/*
   bool noob::joint::get_spring(uint8_t)
   {

   }

   float noob::joint::get_stiffness(uint8_t)
   {

   }

   float noob::joint::get_damping(uint8_t)
   {

   }

   noob::vec2 noob::joint::get_limits(uint8_t)
   {

   }
   */
