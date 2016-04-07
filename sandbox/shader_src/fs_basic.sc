$input v_position, v_normal, mult_normal

#include "common.sh"


void main()
{	
	gl_FragColor = vec4(clamp(colour_0.xyz + get_light(eye_pos.xyz, v_position, v_normal), 0.0, 1.0), 1.0); 
}
