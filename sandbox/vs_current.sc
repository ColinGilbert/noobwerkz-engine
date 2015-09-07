$input a_position, a_normal, a_texcoord
$output v_position, v_normal, v_texcoord

#include "shaders/common.sh"
void main()
{
	v_position = a_position;
	v_normal = a_normal;
	v_texcoord = a_texcoord;
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
