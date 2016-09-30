/*
#pragma once

#include "MathFuncs.hpp"

namespace noob
{
	class camera
	{
		public:
			struct cam_movement
			{
				bool move_left, move_right, move_up, move_down, move_in, move_out, swing_left, swing_right, swing_over, swing_under, roll_clockwise, roll_counterclockwise;
			};
			
			void init(float move_speed, float heading_speed, const noob::vec3& pos);
			void update(double delta, const noob::camera::cam_movement& movement);
			noob::mat4 get_view_matrix() { return view_mat; }
			float cam_speed, cam_heading_speed;
		
		protected:
			double previous_seconds;
			noob::vec4 fwd, rgt, up;
			noob::mat4 T, R, view_mat;
			noob::vec3 cam_pos;
			noob::versor quaternion;
	};
}
*/
