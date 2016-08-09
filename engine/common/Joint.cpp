#include "Joint.hpp"

void noob::joint::init(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4& local_a, const noob::mat4& local_b) noexcept(true) 
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner = new btGeneric6DofSpringConstraint(*(a.inner), *(b.inner), tr_a, tr_b, true);
	inner->setEquilibriumPoint();//0, 0);
	w->addConstraint(inner, true);
}

void noob::joint::set_frames(const noob::mat4& local_a, const noob::mat4& local_b) noexcept(true) 
{
	btTransform tr_a;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	btTransform tr_b;
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner->setFrames(tr_a, tr_b);
}

void noob::joint::set_enabled(uint8_t i, bool val) noexcept(true) 
{
	inner->enableSpring(i, val);
}

void noob::joint::set_stiffness(uint8_t i, float val) noexcept(true) 
{
	inner->setStiffness(i, val);
}

void noob::joint::set_damping(uint8_t i, float val) noexcept(true) 
{
	inner->setDamping(i, val);
}

void noob::joint::set_limits(uint8_t i, const noob::vec2& lims) noexcept(true) 
{
	inner->setLimit(i, lims.v[0], lims.v[1]);
}

bool noob::joint::is_enabled(uint8_t i) const noexcept(true)
{
	
}

float noob::joint::get_stiffness(uint8_t i) const noexcept(true)
{

}

float noob::joint::get_damping(uint8_t i) const noexcept(true)
{

}

noob::vec2 noob::joint::get_limits(uint8_t i) const noexcept(true)
{

}

noob::vec3 noob::joint::get_axis(uint8_t i) const noexcept(true) 
{
	return noob::vec3_from_bullet(inner->getAxis(i));
}

float noob::joint::get_angle(uint8_t i) const noexcept(true)
{
	return inner->getAngle(i);
}

