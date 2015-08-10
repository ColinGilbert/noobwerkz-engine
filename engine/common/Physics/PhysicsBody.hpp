#pragma once

#include "MathFuncs.hpp"
#include "PhysicsShape.hpp"


namespace noob
{
	class physics_body
	{
		public:
			void init(const noob::mat4& transform, const noob::physics_shape& shape, float mass = 1.0f, float friction = 0.2f, float rolling_friction = 0.2f, float restitution = 0.0f);
			
			void set_damping(float linear_damping, float angular_damping);
			void set_sleeping_thresholds(float linear, float angular);
			void set_linear_factor(const noob::vec3& factor);
			void set_angular_factor(const noob::vec3& factor);

			void apply_force(const noob::vec3& force, const noob::vec3& relative_pos = noob::vec3(0.0f, 0.0f, 0.0f));
			void apply_torque(const noob::vec3& torque);

			void apply_impulse(const noob::vec3& impulse, const noob::vec3& relative_pos = noob::vec3(0.0f, 0.0f, 0.0f));
			void apply_torque_impulse(const noob::vec3& torque);

			noob::mat4 get_transform() const;
			btRigidBody* get_raw_ptr() const;
			

		protected:
			btRigidBody* body;
	};
}
