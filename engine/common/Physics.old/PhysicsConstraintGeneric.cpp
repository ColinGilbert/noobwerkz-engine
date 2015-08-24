#include "PhysicsConstraintGeneric.hpp"


void noob::physics_constraint_generic::init(const noob::physics_body& parent, const noob::physics_body& child, const noob::mat4& parent_frame, const noob::mat4& child_frame)
{
	btTransform a;
	a.setFromOpenGLMatrix(&parent_frame.m[0]);
	btTransform b;
	b.setFromOpenGLMatrix(&child_frame.m[0]);
	btGeneric6DofSpring2Constraint* constraint = new btGeneric6DofSpring2Constraint(*parent.get_raw_ptr(), *parent.get_raw_ptr(), a, b);
	constraint->setEquilibriumPoint();
}


void noob::physics_constraint_generic::set_spring(const std::array<bool, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->enableSpring(i, values[i]);
	}
}
			

void noob::physics_constraint_generic::set_motor(const std::array<bool, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->enableMotor(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_servo(const std::array<bool, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setServo(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_target_velocity(const std::array<float, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setTargetVelocity(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_servo_target(const std::array<float, 6>& values)
{	for (size_t i = 0; i < 6; i++)
	{
		constraint->setServoTarget(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_max_motor_force(const std::array<float, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setMaxMotorForce(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_stiffness(const std::array<float, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setStiffness(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_damping(const std::array<float, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setDamping(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_bounce(const std::array<float, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setBounce(i, values[i]);
	}
}


void noob::physics_constraint_generic::set_limits(const std::array<noob::vec2, 6>& values)
{
	for (size_t i = 0; i < 6; i++)
	{
		constraint->setLimit(i, values[i].v[0], values[i].v[1]);
	}
}

btGeneric6DofSpring2Constraint* noob::physics_constraint_generic::get_raw_ptr() const
{
	return constraint;
}
