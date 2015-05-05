$input a_position, a_normal
uniform mat4 u_normalMatrix;

void main()
{
	gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	v_color0 = normalize(mul(u_normalMatrix, vec4(a_normal, 0.0)));
}
