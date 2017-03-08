$input a_position, a_normal
$output v_position, v_normal, mult_normal

#include "common.sh"

// uniform mat4 world_mat;
// uniform mat4 normal_mat;

void main()
{
	v_position = a_position.xyz;
	// v_position = mul(u_modelViewProj, vec4(a_position, 1.0)).xyz; // v_position = mul(world_mat, vec4(a_position, 1.0)).xyz;
	// v_position = mul(normal_mat, vec4(a_position, 1.0)).xyz;

	// v_normal = mul(world_mat, vec4(a_normal, 1.0)).xyz;
	// v_normal = mul(normal_mat, vec4(a_normal, 1.0)).xyz;
	v_normal = a_normal;
	mult_normal = mul(normal_mat, vec4(a_normal, 1.0)).xyz;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
