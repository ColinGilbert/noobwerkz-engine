#include "MathFuncs.hpp"


namespace noob
{
	/*------------------------------VECTOR FUNCTIONS------------------------------*/
	float length(const vec3& v) 
	{
		return sqrt (v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2]);
	}

	vec3 normalize(const vec3& v) 
	{
		vec3 vb;
		float l = length (v);
		if (0.0f == l) {
			return vec3 (0.0f, 0.0f, 0.0f);
		}
		vb.v[0] = v.v[0] / l;
		vb.v[1] = v.v[1] / l;
		vb.v[2] = v.v[2] / l;
		return vb;
	}
	
	float dot(const vec3& a, const vec3& b)
	{
		return a.v[0] * b.v[0] + a.v[1] * b.v[1] + a.v[2] * b.v[2];
	}

	vec3 cross(const vec3& a, const vec3& b)
	{
		float x = a.v[1] * b.v[2] - a.v[2] * b.v[1];
		float y = a.v[2] * b.v[0] - a.v[0] * b.v[2];
		float z = a.v[0] * b.v[1] - a.v[1] * b.v[0];
		return vec3 (x, y, z);
	}

	float get_squared_dist(const vec3& from, const vec3& to)
	{
		float x = (to.v[0] - from.v[0]) * (to.v[0] - from.v[0]);
		float y = (to.v[1] - from.v[1]) * (to.v[1] - from.v[1]);
		float z = (to.v[2] - from.v[2]) * (to.v[2] - from.v[2]);
		return x + y + z;
	}

	/* converts an un-normalized direction into a heading in degrees
	   NB i suspect that the z is backwards here but i've used in in
	   several places like this. d'oh! */
	float direction_to_heading(const vec3& d) 
	{
		return atan2 (-d.v[0], -d.v[2]) * ONE_RAD_IN_DEG;
	}

	vec3 heading_to_direction(float degrees) 
	{
		float rad = degrees * ONE_DEG_IN_RAD;
		return vec3 (-sinf (rad), 0.0f, -cosf (rad));
	}

	/*--------------------------AFFINE MATRIX FUNCTIONS---------------------------*/
	// translate a 4d matrix with xyz array
	mat4 translate(const mat4& m, const vec3& v)
	{
		mat4 m_t = identity_mat4();
		m_t.m[12] = v.v[0];
		m_t.m[13] = v.v[1];
		m_t.m[14] = v.v[2];
		return m_t * m;
	}

	mat4 rotate(const mat4& m, const versor& v)
	{
		return quat_to_mat4(v) * m;
	}

	// rotate around x axis by an angle in degrees
	mat4 rotate_x_deg(const mat4& m, float deg)
	{
		// convert to radians
		float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[5] = cos (rad);
		m_r.m[9] = -sin (rad);
		m_r.m[6] = sin (rad);
		m_r.m[10] = cos (rad);
		return m_r * m;
	}

	// rotate around y axis by an angle in degrees
	mat4 rotate_y_deg(const mat4& m, float deg)
	{
		// convert to radians
		float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[0] = cos (rad);
		m_r.m[8] = sin (rad);
		m_r.m[2] = -sin (rad);
		m_r.m[10] = cos (rad);
		return m_r * m;
	}

	// rotate around z axis by an angle in degrees
	mat4 rotate_z_deg(const mat4& m, float deg)
	{
		// convert to radians
		float rad = deg * ONE_DEG_IN_RAD;
		mat4 m_r = identity_mat4 ();
		m_r.m[0] = cos (rad);
		m_r.m[4] = -sin (rad);
		m_r.m[1] = sin (rad);
		m_r.m[5] = cos (rad);
		return m_r * m;
	}

	// scale a matrix by [x, y, z]
	mat4 scale(const mat4& m, const vec3& v)
	{
		mat4 a = identity_mat4();
		a.m[0] = v.v[0];
		a.m[5] = v.v[1];
		a.m[10] = v.v[2];
		return a * m;
	}


	vec3 get_normal(const std::array<noob::vec3, 3>& vertices)
	{
		// Note: We onlt need the first three points on the face to calculate its normal.
		noob::vec3 u = vertices[1] - vertices[0];
		noob::vec3 v = vertices[2] - vertices[0];
		noob::vec3 results = noob::cross(u, v);
		return normalize(results);
	}


	// --------------------------- EXTRACTION OF COMPONENTS FROM MAT4 ------------------------------*/ 
	/*	vec4 rotation_from_mat4(const mat4& m)
		{

		}
		*/	
	vec3 translation_from_mat4(const mat4& m)
	{
		vec3 v;
		v.v[0] = m.m[12];
		v.v[1] = m.m[13];
		v.v[2] = m.m[14];
		return v;
	}


	vec3 scale_from_mat4(const mat4& m)
	{
		vec3 v;
		v.v[0] = m.m[0];
		v.v[1] = m.m[5];
		v.v[2] = m.m[10];
		return v;
	}

	/*-----------------------VIRTUAL CAMERA MATRIX FUNCTIONS----------------------*/
	// returns a view matrix using the opengl lookAt style. COLUMN ORDER.
	mat4 look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up) 
	{
		// inverse translation
		mat4 p = identity_mat4 ();
		p = translate (p, vec3 (-cam_pos.v[0], -cam_pos.v[1], -cam_pos.v[2]));
		// distance vector
		vec3 d = targ_pos - cam_pos;
		// forward vector
		vec3 f = normalize (d);
		// right vector
		vec3 r = normalize (cross (f, up));
		// real up vector
		vec3 u = normalize (cross (r, f));
		mat4 ori = identity_mat4 ();
		ori.m[0] = r.v[0];
		ori.m[4] = r.v[1];
		ori.m[8] = r.v[2];
		ori.m[1] = u.v[0];
		ori.m[5] = u.v[1];
		ori.m[9] = u.v[2];
		ori.m[2] = -f.v[0];
		ori.m[6] = -f.v[1];
		ori.m[10] = -f.v[2];

		return ori * p;//p * ori;
	}

	// returns a perspective function mimicking the opengl projection style.
	mat4 perspective(float fovy, float aspect, float near, float far)
	{
		float fov_rad = fovy * ONE_DEG_IN_RAD;
		float range = tan (fov_rad / 2.0f) * near;
		float sx = (2.0f * near) / (range * aspect + range * aspect);
		float sy = near / range;
		float sz = -(far + near) / (far - near);
		float pz = -(2.0f * far * near) / (far - near);
		mat4 m = zero_mat4 (); // make sure bottom-right corner is zero
		m.m[0] = sx;
		m.m[5] = sy;
		m.m[10] = sz;
		m.m[14] = pz;
		m.m[11] = -1.0f;
		return m;
	}
	/*
	   void
	   mat4_set_orthographic( mat4 *self,
	   float left,   float right,
	   float bottom, float top,
	   float znear,  float zfar )
	   {
	   assert( self );
	   assert( right  != left );
	   assert( bottom != top  );
	   assert( znear  != zfar );

	   mat4_set_zero( self );

	   self->m00 = +2.0/(right-left);
	   self->m30 = -(right+left)/(right-left);
	   self->m11 = +2.0/(top-bottom);
	   self->m31 = -(top+bottom)/(top-bottom);
	   self->m22 = -2.0/(zfar-znear);
	   self->m32 = -(zfar+znear)/(zfar-znear);
	   self->m33 = 1.0;
	   }

*/

	/*
	   0 4 8  12
	   1 5 9  13
	   2 6 10 14
	   3 7 11 15
	   */
	mat4 ortho(float left, float right, float bottom, float top, float near, float far)
	{
		mat4 m = zero_mat4();
		m.m[0] = 2.0/(right-left);
		m.m[5] = 2.0/(top - bottom);
		m.m[10] = -2.0/(far - near);
		m.m[12] = (right + left)/(right - left);
		m.m[13] = (top + bottom)/(top - bottom);
		m.m[14] = (far + near)/(far - near);
		m.m[15] = 1.0;
		return m;
	}

	versor quat_from_axis_rad(float radians, float x, float y, float z)
	{
		versor result;
		result.q[0] = cos (radians / 2.0);
		result.q[1] = sin (radians / 2.0) * x;
		result.q[2] = sin (radians / 2.0) * y;
		result.q[3] = sin (radians / 2.0) * z;
		return result;
	}

	versor quat_from_axis_deg (float degrees, float x, float y, float z)
	{
		return quat_from_axis_rad (ONE_DEG_IN_RAD * degrees, x, y, z);
	}

	mat4 quat_to_mat4 (const versor& q)
	{
		float w = q.q[0];
		float x = q.q[1];
		float y = q.q[2];
		float z = q.q[3];
		return mat4 (
				1.0f - 2.0f * y * y - 2.0f * z * z,
				2.0f * x * y + 2.0f * w * z,
				2.0f * x * z - 2.0f * w * y,
				0.0f,
				2.0f * x * y - 2.0f * w * z,
				1.0f - 2.0f * x * x - 2.0f * z * z,
				2.0f * y * z + 2.0f * w * x,
				0.0f,
				2.0f * x * z + 2.0f * w * y,
				2.0f * y * z - 2.0f * w * x,
				1.0f - 2.0f * x * x - 2.0f * y * y,
				0.0f,
				0.0f,
				0.0f,
				0.0f,
				1.0f
			    );
	}

	versor normalize(const versor& q)
	{
		// norm(q) = q / magnitude (q)
		// magnitude (q) = sqrt (w*w + x*x...)
		// only compute sqrt if interior sum != 1.0
		versor qq(q);
		float sum = qq.q[0] * qq.q[0] + qq.q[1] * qq.q[1] + qq.q[2] * qq.q[2] + qq.q[3] * qq.q[3];
		// NB: floats have min 6 digits of precision
		const float thresh = 0.0001f;
		if (fabs (1.0f - sum) < thresh)
		{
			return q;
		}
		float mag = sqrt(sum);
		return qq / mag;
	}

	float dot(const versor& q, const versor& r)
	{
		return q.q[0] * r.q[0] + q.q[1] * r.q[1] + q.q[2] * r.q[2] + q.q[3] * r.q[3];
	}

	versor slerp(const versor& q, const versor& r, float t)
	{
		versor temp_q(q);
		// angle between q0-q1
		float cos_half_theta = dot(temp_q, r);
		// as found here http://stackoverflow.com/questions/2886606/flipping-issue-when-interpolating-rotations-using-quaternions
		// if dot product is negative then one quaternion should be negated, to make
		// it take the short way around, rather than the long way
		// yeah! and furthermore Susan, I had to recalculate the d.p. after this

		if (cos_half_theta < 0.0f)
		{
			for (int i = 0; i < 4; i++)
			{
				temp_q.q[i] *= -1.0f;
			}
			cos_half_theta = dot (temp_q, r);
		}
		// if qa=qb or qa=-qb then theta = 0 and we can return qa
		if (fabs(cos_half_theta) >= 1.0f)
		{
			return temp_q;
		}
		// Calculate temporary values
		float sin_half_theta = sqrt(1.0f - cos_half_theta * cos_half_theta);
		// if theta = 180 degrees then result is not fully defined
		// we could rotate around any axis normal to qa or qb
		versor result;
		if (fabs(sin_half_theta) < 0.001f)
		{
			for (int i = 0; i < 4; i++)
			{
				result.q[i] = (1.0f - t) * temp_q.q[i] + t * r.q[i];
			}
			return result;
		}
		float half_theta = acos(cos_half_theta);
		float a = sin((1.0f - t) * half_theta) / sin_half_theta;
		float b = sin(t * half_theta) / sin_half_theta;
		for (int i = 0; i < 4; i++)
		{
			result.q[i] = temp_q.q[i] * a + r.q[i] * b;
		}
		return result;
	}
	
	versor quat_from_mat4(const mat4& m)
	{
		glm::mat4 mm = glm::make_mat4(&m.m[0]);
		glm::quat q = glm::quat_cast(mm);
		noob::versor qq;
		qq.q[0] = q[0];
		qq.q[1] = q[1];
		qq.q[2] = q[2];
		qq.q[3] = q[3];
		return qq;
	}

	vec3 lerp(const noob::vec3& a, const noob::vec3& b, float t) { return a + (b - a) * t; }
}
