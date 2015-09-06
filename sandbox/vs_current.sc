$input a_position, a_normal
$output v_position, v_normal

#include "shaders/common.sh"

void main()
{
	v_position = mul(vec4(u_modelView, a_position), 1.0);
	v_normal = a_normal; 
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
