#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class controls
	{
		public:
			void set_eye_pos(const noob::vec3& arg)
			{
				eye_pos = arg;
			}

			void set_eye_target(const noob::vec3& arg)
			{
				eye_target = arg;
			}

			void set_eye_up(const noob::vec3& arg)
			{
				eye_up = arg;
			}

			void increment_eye_pos(const noob::vec3& arg)
			{
				eye_pos += arg;
			}

			void increment_eye_target(const noob::vec3& arg)
			{
				eye_target += arg;
			}

			void increment_eye_up(const noob::vec3& arg)
			{
				eye_up += arg;
			}

			noob::vec3 get_eye_pos() const
			{
				return eye_pos;
			}

			noob::vec3 get_eye_target() const
			{
				return eye_target;
			}

			noob::vec3 get_eye_up() const
			{
				return eye_up;
			}

		protected:
			noob::vec3 eye_pos, eye_target, eye_up;
	};
}
