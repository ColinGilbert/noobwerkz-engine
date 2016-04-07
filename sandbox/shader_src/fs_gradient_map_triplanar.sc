$input v_position, v_normal, mult_normal

#include "common.sh"

SAMPLER2D(texture_0, 0);

// uniform vec4 colour_0;
// uniform vec4 colour_1;
// uniform vec4 colour_2;
// uniform vec4 colour_3;

// Texture blending factor
// uniform vec4 blend_0;

// Positions are only required for the two mid-gradient colours, as the other two are at the ends
// uniform vec4 blend_1;

// Scaling factors (for the texture)
// uniform vec4 tex_scales;

// uniform texture_0;
// uniform vec4 basic_light_0;
// uniform vec4 basic_light_1;


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

	vec4 xaxis = vec4(texture2D(texture_0, position.yz * tex_scales.x).rgb, 1.0);
	vec4 yaxis = vec4(texture2D(texture_0, position.xz * tex_scales.y).rgb, 1.0);
	vec4 zaxis = vec4(texture2D(texture_0, position.xy * tex_scales.z).rgb, 1.0);

	vec4 tex = xaxis * normal_blend.x + yaxis * normal_blend.y + zaxis * normal_blend.z;

	float tex_r = blend_0.x * tex.r;
	float tex_g = blend_0.y * tex.g;
	float tex_b = blend_0.z * tex.b;

	vec4 tex_weighted = vec4(tex_r, tex_g, tex_b, 1.0);
	float tex_intensity = (tex_r + tex_g + tex_b) * 0.3333;

	float ratio_0_to_1 = when_le(tex_intensity, blend_1.x) * ((tex_intensity + blend_1.x) * 0.5);
	float ratio_1_to_2 = when_le(tex_intensity, blend_1.y) * when_gt(tex_intensity, blend_1.x) * ((tex_intensity + blend_1.y) * 0.5);
	float ratio_2_to_3 = when_ge(tex_intensity, blend_1.y) * ((tex_intensity + 1.0) * 0.5);
	
	vec4 tex_final = ((colour_0 + colour_1) * ratio_0_to_1) + ((colour_1 + colour_2) * ratio_1_to_2) + ((colour_2 + colour_3) * ratio_2_to_3);

	gl_FragColor = clamp(tex_final, 0.0, 1.0);


	// float light_intensity = 0.0;
	// float diffuse_0 = clamp(dot(mult_normal, basic_light_0.xyz), 0.0, 1.0);
	// diffuse_0 *= 0.8;
	// light_intensity += clamp(diffuse_0 + basic_light_0.w, 0.0, 1.0);
	// float diffuse_1 = clamp(dot(mult_normal, basic_light_1.xyz), 0.0, 1.0);
	// diffuse_1 *= 0.8;
	// light_intensity += clamp (light_intensity + diffuse_1 + basic_light_1.w, 0.0, 1.0);
	// light_intensity = clamp(light_intensity, 0.0, 1.0);
	// gl_FragColor = clamp(tex_final * light_intensity, 0.0, 1.0);
}

// float diffuse = clamp(dot(basic_light_0.xyz, v_normal), 0.0, 1.0);
// float diffuse = max(dot(basic_light_0.xyz, v_normal), 0.0);
