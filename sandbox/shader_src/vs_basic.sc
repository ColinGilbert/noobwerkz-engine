$input a_position, a_normal
$output v_position, v_normal, mult_normal

#include "common.sh"

void main()
{
	v_position = a_position.xyz;
	v_normal = a_normal;
	mult_normal = mul(normal_mat, vec4(a_normal, 1.0)).xyz;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
