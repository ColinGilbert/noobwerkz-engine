#pragma once
#include "MathFuncs.hpp"

namespace noob
{
	class camera
	{
		public:
			camera();
			void update(double elapsed_seconds);
			const noob::mat4 get_view_matrix() const { return view_mat; }
			void move_north();
			void move_south();
			void move_east();
			void move_west();
			void move_forward();
			void move_back();
			void pitch_up(); 
			void pitch_down(); 
			void yaw_left(); 
			void yaw_right(); 
			void roll_clockwise();
			void roll_counterclockwise();
			noob::vec3 get_position();

		private:
			float near, far, fovy, aspect, cam_speed, cam_heading_speed, cam_heading;
			noob::mat4 view_mat, T, R;
			float quaternion[4];
			noob::vec4 fwd, rgt, up;
			unsigned int movement_mask;
			noob::vec3 cam_pos;
	};
}
