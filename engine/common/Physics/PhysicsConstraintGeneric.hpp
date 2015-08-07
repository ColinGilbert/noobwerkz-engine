#pragma once

#include "PhysicsBody.hpp"

namespace noob
{
	class physics_constraint_generic
	{
		public:
		
		void init(const noob::physics_body& parent, const noob::physics_body& child, const noob::mat4& parent_frame, const noob::mat4& child_frame);

		void set_spring(const std::array<bool, 6>& values);

		void set_motor(const std::array<bool, 6>& values);

		void set_servo(const std::array<bool, 6>& values);

		void set_target_velocity(const std::array<float, 6>& values);

		void set_servo_target(const std::array<float, 6>& values);

		void set_max_motor_force(const std::array<float, 6>& values);

		void set_stiffness(const std::array<float, 6>& values);

		void set_damping(const std::array<float, 6>& values);

		void set_bounce(const std::array<float, 6>& values);

		void set_limits(const std::array<noob::vec2, 6>& values);

		btGeneric6DofSpring2Constraint* get_raw_ptr() const;

		protected:
			//noob::physics_body parent, child;
			btGeneric6DofSpring2Constraint* constraint;
	};

}
