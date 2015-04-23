#include "Camera.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define CLEAR_FLAGS 0
#define NORTH 1
#define SOUTH 2
#define EAST 4
#define WEST 8
#define FORWARD 16
#define BACK 32
#define PITCH_UP 64
#define PITCH_DOWN 128
#define YAW_LEFT 256
#define YAW_RIGHT 512
#define ROLL_CLOCKWISE 1024
#define ROLL_COUNTERCLOCKWISE 2048


noob::camera::camera()
{
	movement_mask = 0;
	cam_speed = 15.0f; 
	cam_heading_speed = 100.0f; 
	cam_heading = 0.0f;
	cam_pos = noob::vec3(0.0f, 0.0f, 80.0f);
	T = noob::translate(noob::identity_mat4(), noob::vec3(-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2]));
	noob::create_versor(quaternion, -cam_heading, 0.0f, 1.0f, 0.0f);
	noob::quat_to_mat4(R.m, quaternion);
	view_mat = R * T;
	fwd = noob::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	rgt = noob::vec4(1.0f, 0.0f, 0.0f, 0.0f);
	up = noob::vec4(0.0f, 1.0f, 0.0f, 0.0f);	
}

void noob::camera::update(double elapsed_seconds)
{
	bool cam_moved = false;
	noob::vec3 move(0.0, 0.0, 0.0);
	float cam_yaw = 0.0f;
	float cam_pitch = 0.0f;
	float cam_roll = 0.0;

	if (movement_mask & WEST)
	{
		move.v[0] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & EAST)
	{
		move.v[0] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & NORTH)
	{
		move.v[1] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & SOUTH)
	{
		move.v[1] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & FORWARD)
	{
		move.v[2] -= cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & BACK)
	{
		move.v[2] += cam_speed * elapsed_seconds;
		cam_moved = true;
	}
	if (movement_mask & YAW_LEFT)
	{
		cam_yaw += cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_yaw[16];
		noob::create_versor(q_yaw, cam_yaw, up.v[0], up.v[1], up.v[2]);
		noob::mult_quat_quat(quaternion, q_yaw, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (movement_mask & YAW_RIGHT)
	{
		cam_yaw -= cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_yaw[16];
		noob::create_versor(q_yaw, cam_yaw, up.v[0], up.v[1], up.v[2]);
		noob::mult_quat_quat(quaternion, q_yaw, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (movement_mask & PITCH_UP) 
	{
		cam_pitch += cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_pitch[16];
		noob::create_versor(q_pitch, cam_pitch, rgt.v[0], rgt.v[1], rgt.v[2]);
		noob::mult_quat_quat(quaternion, q_pitch, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (movement_mask & PITCH_DOWN)
	{
		cam_pitch -= cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_pitch[16];
		noob::create_versor(q_pitch, cam_pitch, rgt.v[0], rgt.v[1], rgt.v[2]);
		noob::mult_quat_quat(quaternion, q_pitch, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (movement_mask & ROLL_CLOCKWISE) 
	{
		cam_roll -= cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_roll[16];
		noob::create_versor(q_roll, cam_roll, fwd.v[0], fwd.v[1], fwd.v[2]);
		noob::mult_quat_quat(quaternion, q_roll, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (movement_mask & ROLL_COUNTERCLOCKWISE) 
	{
		cam_roll += cam_heading_speed * elapsed_seconds;
		cam_moved = true;
		float q_roll[16];
		noob::create_versor(q_roll, cam_roll, fwd.v[0], fwd.v[1], fwd.v[2]);
		noob::mult_quat_quat(quaternion, q_roll, quaternion);
		noob::quat_to_mat4(R.m, quaternion);
		fwd = R * noob::vec4(0.0, 0.0, -1.0, 0.0);
		rgt = R * noob::vec4(1.0, 0.0, 0.0, 0.0);
		up = R * noob::vec4(0.0, 1.0, 0.0, 0.0);
	}
	if (cam_moved)
	{
		noob::quat_to_mat4(R.m, quaternion);

		// checking for fp errors
		//	printf ("dot fwd . up %f\n", dot (fwd, up));
		//	printf ("dot rgt . up %f\n", dot (rgt, up));
		//	printf ("dot fwd . rgt\n %f", dot (fwd, rgt));

		cam_pos = cam_pos + noob::vec3(fwd) * -move.v[2];
		cam_pos = cam_pos + noob::vec3(up) * move.v[1];
		cam_pos = cam_pos + noob::vec3(rgt) * move.v[0];
		noob::mat4 T = noob::translate(noob::identity_mat4(), noob::vec3(cam_pos));

		view_mat = noob::inverse(R) * noob::inverse(T);

		movement_mask = CLEAR_FLAGS;
	}
}

void noob::camera::move_north() { movement_mask = movement_mask | NORTH; }
void noob::camera::move_south() { movement_mask = movement_mask | SOUTH; }
void noob::camera::move_east() { movement_mask = movement_mask | EAST; }
void noob::camera::move_west() { movement_mask = movement_mask | WEST; }
void noob::camera::move_forward() { movement_mask = movement_mask | FORWARD; }
void noob::camera::move_back() { movement_mask = movement_mask | BACK; }
void noob::camera::pitch_up() { movement_mask = movement_mask | PITCH_UP; }
void noob::camera::pitch_down() { movement_mask = movement_mask | PITCH_DOWN; }
void noob::camera::yaw_left() { movement_mask = movement_mask | YAW_LEFT; }
void noob::camera::yaw_right() { movement_mask = movement_mask | YAW_RIGHT; }
void noob::camera::roll_clockwise() { movement_mask = movement_mask | ROLL_CLOCKWISE; }
void noob::camera::roll_counterclockwise() { movement_mask = movement_mask | ROLL_COUNTERCLOCKWISE; }
noob::vec3 noob::camera::get_position() { return cam_pos; }
