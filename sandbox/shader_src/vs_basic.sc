$input a_position, a_normal
$output v_position, v_normal, world_pos, world_normal

#include "common.sh"

void main()
{
	v_position = a_position.xyz;
	v_normal = a_normal;

	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	
	world_pos = mul(u_model[0], vec4(v_position, 1.0)).xyz;
	world_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;
}
