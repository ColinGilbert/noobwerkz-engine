$input a_position, a_normal
$output v_position, v_normal

#include "common.sh"

void main()
{
	v_position = a_position;
	v_normal = a_normal; 
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
