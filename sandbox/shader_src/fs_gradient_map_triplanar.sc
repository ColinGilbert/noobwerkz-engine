$input v_position, v_normal

#include "common.sh"

SAMPLER2D(u_texture, 0);

uniform vec4 colour_1;
uniform vec4 colour_2;
uniform vec4 colour_3;
uniform vec4 colour_4;

uniform vec4 mapping_blend;

// Positions are only required for the two mid-gradient colours, as the other two are at the ends
uniform vec4 colour_positions;

// Scaling factors (for the texture)
uniform vec4 scales;

// uniform u_texture;
uniform vec4 light_0_direction;
uniform vec4 light_1_direction;
uniform vec4 light_2_direction;
uniform vec4 light_3_direction;


void main()
{
	// vec3 position = normalize(v_position);
	vec3 position = v_position;
	vec3 normal_blend = normalize(max(abs(v_normal), 0.0001));

	float b = normal_blend.x + normal_blend.y + normal_blend.z;
	normal_blend /= b;

	// Test RGB-only. Uncomment for great wisdom!
	// vec4 xaxis = vec4(1.0, 0.0, 0.0, 1.0); 
	// vec4 yaxis = vec4(0.0, 1.0, 0.0, 1.0);
	// vec4 zaxis = vec4(0.0, 0.0, 1.0, 1.0);

	vec4 xaxis = vec4(texture2D(u_texture, position.yz * scales.x).rgb, 1.0);
	vec4 yaxis = vec4(texture2D(u_texture, position.xz * scales.y).rgb, 1.0);
	vec4 zaxis = vec4(texture2D(u_texture, position.xy * scales.z).rgb, 1.0);


	vec4 tex = xaxis * normal_blend.x + yaxis * normal_blend.y + zaxis * normal_blend.z;

	float tex_r = mapping_blend.x * tex.r;
	float tex_b = mapping_blend.y * tex.g;
	float tex_g = mapping_blend.z * tex.b;

	vec4 tex_weighted = vec4(tex_r, tex_g, tex_b, 1.0);
	float tex_intensity = (tex_r + tex_g + tex_b) * 0.3333;

	float ratio_1_to_2 = when_le(tex_intensity, colour_positions.x) * ((tex_intensity + colour_positions.x) * 0.5);
	float ratio_2_to_3 = when_le(tex_intensity, colour_positions.y) * when_gt(tex_intensity, colour_positions.x) * ((tex_intensity + colour_positions.y) * 0.5);
	float ratio_3_to_4 = when_ge(tex_intensity, colour_positions.y) * ((tex_intensity + 1.0) * 0.5);
	
	vec4 tex_final = ((colour_1 + colour_2) * ratio_1_to_2) + ((colour_2 + colour_3) * ratio_2_to_3) + ((colour_3 + colour_4) * ratio_3_to_4);

	// vec3 lightDir = vec3(0.0, 0.0, 1.0);
	// float diffuse = clamp(dot(lightDir, normalFromVS), 0.0, 1.0);
	// diffuse *= 0.7; // Dim the diffuse a bit
	// float ambient = 0.3; // Add some ambient
	// float lightIntensity = diffuse + ambient; // Compute the final light intensity
	// outputColor = surfaceColor * lightIntensity; //Compute final rendered color

	float diffuse = clamp(dot(v_normal, light_0_direction.xyz), 0.0, 1.0);
	// float diffuse = clamp(dot(light_0_direction.xyz, v_normal), 0.0, 1.0);
	// float diffuse = max(dot(light_0_direction.xyz, v_normal), 0.0);
	diffuse *= 0.8;
	float ambient = 0.5;
	float light_intensity = diffuse + ambient;

	gl_FragColor = tex_final * light_intensity;
}
