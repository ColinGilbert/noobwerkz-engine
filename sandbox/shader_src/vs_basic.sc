$input a_position
$output v_position

#include "common.sh"

void main()
{
	v_position = a_position;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
