$input a_position, a_normal, i_data0, i_data1, i_data2, i_data3
$output world_pos, world_normal

#include "common.sh"

void main()
{
	// MINE (NEW):
	v_position

	mat4 model
	model[0] = i_data0;
	model[1] = i_data1;
	model[2] = i_data2;
	model[3] = i_data3;
	
	vec4 i_position = instMul(model[0], vec4(a_position, 1.0));
	gl_Position = mul(u_viewProj, i_position));
	world_pos = mul(u_model[0], i_position).xyz;
	world_normal = mul(u_model[0], vec4(a_normal, 1.0)).xyz;

	// TEXTBOOK:
	// mat4 model
	// model[0] = i_data0;
	// model[1] = i_data1;
	// model[2] = i_data2;
	// model[3] = i_data3;
	
	// vec4 instanced_world_position = instMul(model, vec4(a_position, 1.0));
	// gl_Position = mul(u_viewProj, instanced_world_position);

	// MINE (OLD):
	// v_position = a_position.xyz;
	// v_normal = a_normal;
	// gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));
	// world_pos = mul(u_model[0], vec4(v_position, 1.0)).xyz;
	// world_normal = mul(u_model[0], vec4(a_normal, 0.0)).xyz;
}
