$input a_position, a_normal, a_texcoord0

#include "shaders/common.sh"

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
}
