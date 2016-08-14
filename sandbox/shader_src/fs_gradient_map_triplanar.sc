$input v_position, v_normal, world_pos, world_normal// , world_eye

#include "common.sh"

SAMPLER2D(texture_0, 0);

void main()
{
	// vec3 position = normalize(v_position);
	vec3 position = mul(v_position, model_scales.xyz);
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
	float tex_falloff = (tex_r + tex_g + tex_b) * 0.3333;

	float ratio_0_to_1 = when_le(tex_falloff, blend_1.x) * ((tex_falloff + blend_1.x) * 0.5);
	float ratio_1_to_2 = when_le(tex_falloff, blend_1.y) * when_gt(tex_falloff, blend_1.x) * ((tex_falloff + blend_1.y) * 0.5);
	float ratio_2_to_3 = when_ge(tex_falloff, blend_1.y) * ((tex_falloff + 1.0) * 0.5);

	vec4 tex_final = ((colour_0 + colour_1) * ratio_0_to_1) + ((colour_1 + colour_2) * ratio_1_to_2) + ((colour_2 + colour_3) * ratio_2_to_3);
	
	vec3 light = get_light(world_pos, world_normal);
	vec3 total_colour = tex_final.xyz + light;
	
	gl_FragColor.xyz = clamp(total_colour * u_ambient.xyz, 0.0, 1.0);
	gl_FragColor.w = tex_final.w;
}
