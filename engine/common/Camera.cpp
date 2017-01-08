#include "Camera.hpp"

void noob::camera::init(float move_speed, float heading_speed, const noob::vec3f& pos)
{
	cam_speed = move_speed;
	cam_heading_speed = heading_speed;
	cam_pos = pos;//std::copy(initial_pos.v.begin(), initial_pos.v.end(), cam_pos.v.begin());
	T = noob::translate(noob::identity_mat4<float>(), noob::vec3f(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	noob::versorf q;
	float initial_heading = 0.0;
	q = noob::versor_from_axis_rad<float>(-initial_heading, 0.0, 1.0, 0.0);
	R = noob::versor_to_mat4(q);
	view_mat = R * T;
	fwd = noob::vec4f(0.0, 0.0, -1.0, 0.0);
	rgt = noob::vec4f(1.0, 0.0, 0.0, 0.0);
	up = noob::vec4f(0.0, 1.0, 0.0, 0.0);
}

void noob::camera::update(double delta, const noob::camera::cam_movement& movement)
{
	bool cam_moved = false;
	
	noob::vec3f move(0.0, 0.0, 0.0);
	
	float cam_yaw = 0.0f; // y-rotation in degrees
	float cam_pitch = 0.0f;
	float cam_roll = 0.0;
	
	if (movement.move_left)
	{
		move[0] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_right) 
	{
		move[0] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_up) 
	{
		move[1] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_down) 
	{
		move[1] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_in)
	{
		move[2] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_out)
	{
		move[2] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.swing_left)
	{
		cam_yaw += cam_heading_speed * delta;
		cam_moved = true;

		noob::versorf q_yaw = noob::versor_from_axis_rad<float>(cam_yaw, up[0], up[1], up[2]);
		quaternion = q_yaw * quaternion;

		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_right)
	{
		cam_yaw -= cam_heading_speed * delta;
		cam_moved = true;
		noob::versorf q_yaw = noob::versor_from_axis_rad<float>(cam_yaw, up[0], up[1], up[2]);
		quaternion = q_yaw * quaternion;

		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_over)
	{
		cam_pitch += cam_heading_speed * delta;
		cam_moved = true;
		noob::versorf q_pitch = noob::versor_from_axis_rad<float>(cam_pitch, rgt[0], rgt[1], rgt[2]);
		quaternion = q_pitch * quaternion;

		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_under)
	{
		cam_pitch -= cam_heading_speed * delta;
		cam_moved = true;
		noob::versorf q_pitch = noob::versor_from_axis_rad<float>(cam_pitch, rgt[0], rgt[1], rgt[2]);
		quaternion = q_pitch * quaternion;

		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.roll_clockwise)
	{
		cam_roll -= cam_heading_speed * delta;
		cam_moved = true;
		noob::versorf q_roll = noob::versor_from_axis_rad<float>(cam_roll, fwd[0], fwd[1], fwd[2]);
		quaternion = q_roll * quaternion;

		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.roll_counterclockwise)
	{
		cam_roll += cam_heading_speed * delta;
		cam_moved = true;
		noob::versorf q_roll = noob::versor_from_axis_rad<float>(cam_roll, fwd[0], fwd[1], fwd[2]);
		quaternion = q_roll * quaternion;


		R = noob::versor_to_mat4(quaternion);
		fwd = R * noob::vec4f(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4f(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4f(0.0, 1.0, 0.0, 0.0);
	}
	
	if (cam_moved)
	{
		R = noob::versor_to_mat4(quaternion);

		cam_pos = cam_pos + (noob::vec3f(fwd[0], fwd[1], fwd[2]) * -move.v[2]);
		cam_pos = cam_pos + (noob::vec3f(up[0], up[1], up[2]) * move.v[1]);
		cam_pos = cam_pos + (noob::vec3f(rgt[0], rgt[1], rgt[2]) * move.v[0]);
		
		noob::mat4f T = noob::translate(noob::identity_mat4<float>(), noob::vec3f(cam_pos));

		view_mat = noob::inverse(R) * noob::inverse(T);
	}
}
