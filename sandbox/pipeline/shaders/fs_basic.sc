$input v_position, v_normal, world_pos, world_normal


#include "common.sh"

void main()
{	
	vec3 light = get_light(world_pos, world_normal);
	vec3 total_colour = colour_0.xyz + light;
	gl_FragColor.xyz = clamp(total_colour * u_ambient.xyz, 0.0, 1.0);
	gl_FragColor.w = 1.0;
}
