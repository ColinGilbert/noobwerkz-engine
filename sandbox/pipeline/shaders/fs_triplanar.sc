$input v_position, v_normal

#include "common.sh"

SAMPLER2D(u_texture, 0);

void main()
{
	vec3 position = normalize(v_position);
	vec3 blending = normalize(max(abs(v_normal), 0.0001));

	float b = blending.x + blending.y + blending.z;
	blending /= b; // vec3(b, b, b);

/*	// Test colour-only blending
	vec4 xaxis = vec4(1.0, 0.0, 0.0, 1.0); 
	vec4 yaxis = vec4(0.0, 1.0, 0.0, 1.0);
	vec4 zaxis = vec4(0.0, 0.0, 1.0, 1.0);
*/

	vec4 xaxis = vec4(texture2D(u_texture, position.yz).rgb, 1.0);
	vec4 yaxis = vec4(texture2D(u_texture, position.xz).rgb, 1.0);
	vec4 zaxis = vec4(texture2D(u_texture, position.xy).rgb, 1.0);

	vec4 tex = xaxis * blending.x + yaxis * blending.y + zaxis * blending.z;

	gl_FragColor = tex; 
}
