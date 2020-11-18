#pragma once

// External libs
#include <noob/component/component.hpp>
#include <btBulletDynamicsCommon.h>

// Internal project-related files
#include "Body.hpp"
#include "Shape.hpp"

namespace noob
{
	class constraint
	{
		public:
			enum class type { FIXED = 0, POINT_TO_POINT = 1, HINGE = 2, SLIDE = 3, CONICAL = 4, GEAR = 5, GENERIC = 6 };
			
			
			// ....................................................
			// Initializers
			// TODO: Fix abstraction leak wrt. Bullet physics world
			// ....................................................
			
			void init_fixed(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);
			void init_point(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::vec3f& pivot_a, const noob::vec3f& pivot_b) noexcept(true);
			void init_hinge(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::vec3f& pivot_in_a, const noob::vec3f& pivot_in_b, const noob::vec3f& axis_in_a, const noob::vec3f& axis_in_b) noexcept(true);
			void init_slide(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);
	//		void init_conical() noexcept(true);
	//		void init_gear() noexcept (true);
			void init_generic(btDynamicsWorld* const w, const noob::body& a, const noob::body& b, const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);
			// ..........................................
			// Methods for all constraints
			// ..........................................
			void enable() noexcept(true);
			void disable() noexcept(true);
			void set_breaking_impulse_threshold(float) noexcept(true);
			
			bool is_enabled() noexcept(true);
			float get_breaking_threshold() noexcept(true);

			void set_bounce(int index, float bounce);
			void set_stiffness(int index, float stiffness);
			void set_damping(int index, float damping);

			// ..........................................
			// Methods for hinge constraints (and also slide constraints)
			// TODO: Add motor-related methods
			// TODO: Double-check to see whether or not bias and relaxation are the same thing as restitution and damping. If so, merge method names.
			// ..........................................
			bool set_angular_limits(float lower, float upper) noexcept(true);
			/*
			bool set_angular_limit_softness(float) noexcept(true);
			bool set_angular_bias(float) noexcept(true);
			bool set_angular_relaxation(float) noexcept(true);

			std::tuple<bool, float> get_angular_position() noexcept(true);
			std::tuple<bool, float> get_angular_upper_limit() noexcept(true);
			std::tuple<bool, float> get_angular_lower_limit() noexcept(true);
			
			std::tuple<bool, float> get_angular_limit_softness() noexcept(true);
			std::tuple<bool, float> get_angular_bias() noexcept(true);
			std::tuple<bool, float> get_angular_relaxation() noexcept(true);
		*/

			// ..........................................
			// Methods for slide constraints
			// ..........................................
			bool set_linear_upper_limit(float) noexcept(true);
			bool set_linear_lower_limit(float) noexcept(true);
		/*	
			bool set_linear_limit_softness(float) noexcept(true);
			bool set_linear_limit_restitution(float) noexcept(true);
			bool set_linear_limit_damping(float) noexcept(true);
			// bool set_angular_limit_softness(float) noexcept(true);
			bool set_angular_limit_restitution(float) noexcept(true);
			bool set_angular_limit_damping(float) noexcept(true);

			bool set_linear_limit_softness_ortho(float) noexcept(true);
			bool set_linear_limit_restitution_ortho(float) noexcept(true);
			bool set_linear_limit_damping_ortho(float) noexcept(true);
			bool set_angular_limit_softness_ortho(float) noexcept(true);
			bool set_angular_limit_restitution_ortho(float) noexcept(true);
			bool set_angular_limit_damping_ortho(float) noexcept(true);

			std::tuple<bool, float> get_linear_position() noexcept(true);
			std::tuple<bool, float> get_linear_upper_limit() noexcept(true);
			std::tuple<bool, float> get_linear_lower_limit() noexcept(true);

			std::tuple<bool, float> set_linear_limit_softness() noexcept(true);
			std::tuple<bool, float> set_linear_limit_restitution() noexcept(true);
			std::tuple<bool, float> set_linear_limit_damping() noexcept(true);
			std::tuple<bool, float> set_angular_limit_softness() noexcept(true);
			std::tuple<bool, float> set_angular_limit_restitution() noexcept(true);
			std::tuple<bool, float> set_angular_limit_damping() noexcept(true);

			std::tuple<bool, float> set_linear_limit_softness_ortho() noexcept(true);
			std::tuple<bool, float> set_linear_limit_restitution_ortho() noexcept(true);
			std::tuple<bool, float> set_linear_limit_damping_ortho() noexcept(true);
			std::tuple<bool, float> set_angular_limit_softness_ortho() noexcept(true);
			std::tuple<bool, float> set_angular_limit_restitution_ortho() noexcept(true);
			std::tuple<bool, float> set_angular_limit_damping_ortho() noexcept(true);
*/

			// std::tuple<bool, float> get_linear_depth() noexcept(true);



			// ..........................................
			// Methods for conical constraints
			// ..........................................

			// ..........................................
			// Methods for gear constraints
			// ..........................................
			
			// ..........................................
			// Methods for generic 6DOF constraints
			// ..........................................
			//set_frames(const noob::mat4f& local_a, const noob::mat4f& local_b) noexcept(true);

		
			// bool is_enabled(uint8_t) const noexcept(true);
			
			// noob::vec3f get_axis(uint8_t) const noexcept(true);
			// float get_angle(uint8_t) const noexcept(true);

		protected:
			btTypedConstraint* inner;
			noob::constraint::type inner_type;
	};

	typedef noob::handle<noob::constraint> constraint_handle;
}
