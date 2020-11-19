#include "Constraint.hpp"



void noob::constraint::init_fixed(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);

	inner = new btFixedConstraint(*(a.inner), *(b.inner), tr_a, tr_b);
	// static_cast<btFixedConstraint*>(inner)->setEquilibriumPoint();//0, 0);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::FIXED;

}

void noob::constraint::init_point(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::vec3f& pivot_a, const noob::vec3f& pivot_b) noexcept(true)
{
	inner = new btPoint2PointConstraint(*(a.inner), *(b.inner), noob::vec3f_to_bullet(pivot_a), noob::vec3f_to_bullet(pivot_b));
	// static_cast<btFixedConstraint*>(inner)->setEquilibriumPoint();//0, 0);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::POINT_TO_POINT;
}



void noob::constraint::init_hinge(btDynamicsWorld* const w, const noob::body& a, const noob::vec3f& pivot, const noob::vec3f& axis) noexcept(true)
{
	inner = new btHingeConstraint(*(a.inner), noob::vec3f_to_bullet(pivot), noob::vec3f_to_bullet(axis), true);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::HINGE;
}


void noob::constraint::init_hinge(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::vec3f& pivot_in_a, const noob::vec3f& pivot_in_b, const noob::vec3f& axis_in_a, const noob::vec3f& axis_in_b) noexcept(true)
{
	inner = new btHingeConstraint(*(a.inner), *(b.inner), noob::vec3f_to_bullet(pivot_in_a), noob::vec3f_to_bullet(pivot_in_b), noob::vec3f_to_bullet(axis_in_a), noob::vec3f_to_bullet(axis_in_b), true);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::HINGE;
}

void noob::constraint::init_slide(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner = new btSliderConstraint(*(a.inner), *(b.inner), tr_a, tr_b, true);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::SLIDE;
}

void noob::constraint::init_conical(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner = new btConeTwistConstraint(*(a.inner), *(b.inner), tr_a, tr_b);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::CONICAL;
}
// void init_gear() noexcept (true);

void noob::constraint::init_generic(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true)
{
	btTransform tr_a, tr_b;
	tr_a.setFromOpenGLMatrix(&local_a.m[0]);
	tr_b.setFromOpenGLMatrix(&local_b.m[0]);
	inner = new btGeneric6DofSpringConstraint(*(a.inner), *(b.inner), tr_a, tr_b, true);
	static_cast<btGeneric6DofSpringConstraint*>(inner)->setEquilibriumPoint();//0, 0);
	w->addConstraint(inner, true);
	inner_type = noob::constraint::type::GENERIC;
}

void noob::constraint::enable() noexcept(true)
{
	inner->setEnabled(true);
}

void noob::constraint::disable() noexcept(true)
{
	inner->setEnabled(false);
}

void noob::constraint::set_breaking_impulse_threshold(float arg) noexcept(true)
{
	inner->setBreakingImpulseThreshold(arg);
}


bool noob::constraint::is_enabled() noexcept(true)
{
	return inner->isEnabled();
}

float noob::constraint::get_breaking_threshold() noexcept(true)
{
	return inner->getBreakingImpulseThreshold();
}

void noob::constraint::set_bounce(int index, float bounce)
{
	if(inner_type == noob::constraint::type::POINT_TO_POINT || inner_type == noob::constraint::type::GENERIC)
	{
		static_cast<btGeneric6DofSpring2Constraint*>(inner)->setBounce(index, bounce);
	}
}
void noob::constraint::set_stiffness(int index, float stiffness)
{
	if(inner_type == noob::constraint::type::POINT_TO_POINT || inner_type == noob::constraint::type::GENERIC)
	{
		static_cast<btGeneric6DofSpring2Constraint*>(inner)->setStiffness(index, stiffness);
	}
}

void noob::constraint::set_damping(int index, float damping)
{
	if(inner_type == noob::constraint::type::POINT_TO_POINT || inner_type == noob::constraint::type::GENERIC)
	{
		static_cast<btGeneric6DofSpring2Constraint*>(inner)->setDamping(index, damping);
	}
}

// ..........................................
// Methods for hinge constraints (and also slide constraints)
// TODO: Add motor-related methods
// TODO: Double-check to see whether or not bias and relaxation are the same thing as restitution and damping. If so, merge method names.
// ..........................................
bool noob::constraint::set_angular_limits(float lower, float upper) noexcept(true)
{
	switch (inner_type)
	{
		case (noob::constraint::type::HINGE):
			{
				static_cast<btHingeConstraint*>(inner)->setLimit(lower, upper);
				return true;
			}
		case (noob::constraint::type::SLIDE):
			{
				static_cast<btSliderConstraint*>(inner)->setLowerAngLimit(lower);
				static_cast<btSliderConstraint*>(inner)->setUpperAngLimit(upper);
				return true;
			}
		default: return false;
	}
	return false;
}
/*
   bool noob::constraint::set_angular_limit_softness(float) noexcept(true)
   {
   if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
   }

   bool noob::constraint::set_angular_bias(float) noexcept(true)
   {
//if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

bool noob::constraint::set_angular_relaxation(float) noexcept(true)
{
//if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_position() noexcept(true)
{
if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_upper_limit() noexcept(true)
{
if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_lower_limit() noexcept(true)
{
if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_limit_softness() noexcept(true)
{
if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_bias() noexcept(true)
{
// if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}

std::tuple<bool, float> noob::constraint::get_angular_relaxation() noexcept(true)
{
// if (type == noob::constraint::type::HINGE || type == noob::constraint::type::SLIDE) return false;
}
 */


// ..........................................
// Methods for slide constraints
// ..........................................
bool noob::constraint::set_linear_upper_limit(float arg) noexcept(true)
{
	if (inner_type == noob::constraint::type::SLIDE)
	{
		static_cast<btSliderConstraint*>(inner)->setUpperLinLimit(arg);
	}
	return false;
}

bool noob::constraint::set_linear_lower_limit(float arg) noexcept(true)
{

	if (inner_type == noob::constraint::type::SLIDE)
	{
		static_cast<btSliderConstraint*>(inner)->setLowerLinLimit(arg);
	}
	return false;
}

/*
   bool noob::constraint::set_linear_limit_softness(float) noexcept(true)
   {

   }

   bool noob::constraint::set_linear_limit_restitution(float) noexcept(true)
   {

   }

   bool noob::constraint::set_linear_limit_damping(float) noexcept(true)
   {

   }

// bool noob::constraint::set_angular_limit_softness(float) noexcept(true);

bool noob::constraint::set_angular_limit_restitution(float) noexcept(true)
{

}

bool noob::constraint::set_angular_limit_damping(float) noexcept(true)
{

}

bool noob::constraint::set_linear_limit_softness_ortho(float) noexcept(true)
{

}

bool noob::constraint::set_linear_limit_restitution_ortho(float) noexcept(true)
{

}

bool noob::constraint::set_linear_limit_damping_ortho(float) noexcept(true)
{

}

bool noob::constraint::set_angular_limit_softness_ortho(float) noexcept(true)
{

}

bool noob::constraint::set_angular_limit_restitution_ortho(float) noexcept(true)
{

}

bool noob::constraint::set_angular_limit_damping_ortho(float) noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::get_linear_position() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::get_linear_upper_limit() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::get_linear_lower_limit() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_softness() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_restitution() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_damping() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_softness() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_restitution() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_damping() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_softness_ortho() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_restitution_ortho() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_linear_limit_damping_ortho() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_softness_ortho() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_restitution_ortho() noexcept(true)
{

}

std::tuple<bool, float> noob::constraint::set_angular_limit_damping_ortho() noexcept(true)
{

}
*/
