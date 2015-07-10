/*
#include "Camera.hpp"

void noob::camera::init(float move_speed, float heading_speed, const noob::vec3& initial_pos)
{
	cam_speed = move_speed;
	cam_heading_speed = heading_speed;
	std::copy(initial_pos.v.begin(), initial_pos.v.end(), cam_pos.v.begin());
	T = noob::translate(noob::identity_mat4(), noob::vec3(-cam_pos[0], -cam_pos[1], -cam_pos[2]));
	noob::versor q;
	float initial_heading = 0.0;
	noob::create_versor(&q.q[0], -initial_heading, 0.0, 1.0, 0.0);
	R = noob::quat_to_mat4(q);
	view_mat = R * T;
	fwd = noob::vec4(0.0, 0.0, -1.0, 0.0);
	rgt = noob::vec4(1.0, 0.0, 0.0, 0.0);
	up = noob::vec4(0.0, 1.0, 0.0, 0.0);
}

void noob::camera::update(double delta, const noob::camera::cam_movement& movement)
{
	bool cam_moved = false;
	
	noob::vec3 move(0.0, 0.0, 0.0);
	
	float cam_yaw = 0.0f; // y-rotation in degrees
	float cam_pitch = 0.0f;
	float cam_roll = 0.0;
	
	if (movement.move_left)
	{
		move.v[0] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_right) 
	{
		move.v[0] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_up) 
	{
		move.v[1] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_down) 
	{
		move.v[1] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_in)
	{
		move.v[2] -= cam_speed * delta;
		cam_moved = true;
	}

	if (movement.move_out)
	{
		move.v[2] += cam_speed * delta;
		cam_moved = true;
	}

	if (movement.swing_left)
	{
		cam_yaw += cam_heading_speed * delta;
		cam_moved = true;

		// create a quaternion representing change in heading (the yaw)
		float q_yaw[16];
		noob::create_versor(q_yaw, cam_yaw, up.v[0], up.v[1], up.v[2]);
		// add yaw rotation to the camera's current orientation
		noob::mult_quat_quat(&quaternion.q[0], q_yaw, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_right)
	{
		cam_yaw -= cam_heading_speed * delta;
		cam_moved = true;
		float q_yaw[16];
		noob::create_versor(q_yaw, cam_yaw, up.v[0], up.v[1], up.v[2]);
		noob::mult_quat_quat(&quaternion.q[0], q_yaw, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_over)
	{
		cam_pitch += cam_heading_speed * delta;
		cam_moved = true;
		float q_pitch[16];
		noob::create_versor(q_pitch, cam_pitch, rgt.v[0], rgt.v[1], rgt.v[2]);
		noob::mult_quat_quat(&quaternion.q[0], q_pitch, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.swing_under)
	{
		cam_pitch -= cam_heading_speed * delta;
		cam_moved = true;
		float q_pitch[16];
		noob::create_versor(q_pitch, cam_pitch, rgt.v[0], rgt.v[1], rgt.v[2]);
		noob::mult_quat_quat(&quaternion.q[0], q_pitch, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.roll_clockwise)
	{
		cam_roll -= cam_heading_speed * delta;
		cam_moved = true;
		float q_roll[16];
		noob::create_versor(q_roll, cam_roll, fwd.v[0], fwd.v[1], fwd.v[2]);
		noob::mult_quat_quat(&quaternion.q[0], q_roll, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}

	if (movement.roll_counterclockwise)
	{
		cam_roll += cam_heading_speed * delta;
		cam_moved = true;
		float q_roll[16];
		noob::create_versor(q_roll, cam_roll, fwd.v[0], fwd.v[1], fwd.v[2]);
		noob::mult_quat_quat(&quaternion.q[0], q_roll, &quaternion.q[0]);

		// recalc axes to suit new orientation
		R = noob::quat_to_mat4(quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	
	if (cam_moved)
	{
		R = noob::quat_to_mat4(quaternion);

		cam_pos = cam_pos + noob::vec3(fwd) * -move.v[2];
		cam_pos = cam_pos + noob::vec3(up) * move.v[1];
		cam_pos = cam_pos + noob::vec3(rgt) * move.v[0];
		
		noob::mat4 T = noob::translate(noob::identity_mat4(), noob::vec3(cam_pos));

		view_mat = noob::inverse(R) * noob::inverse(T);
	}
}
*/
