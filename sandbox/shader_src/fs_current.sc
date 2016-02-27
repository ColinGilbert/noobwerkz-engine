$input v_position, v_normal, mult_normal

#include "common.sh"

SAMPLER2D(texture_0, 0);

uniform vec4 colour_0;
uniform vec4 colour_1;
uniform vec4 colour_2;
uniform vec4 colour_3;

// Texture blending factor
uniform vec4 blend_0;

// Positions are only required for the two mid-gradient colours, as the other two are at the ends
uniform vec4 blend_1;

// Scaling factors (for the texture)
uniform vec4 tex_scales;

// Lighting
uniform vec4 eye_pos;

uniform vec4 global_ambient;

uniform vec4 emissive_coeff_0;
uniform vec4 diffuse_coeff_0;
uniform vec4 ambient_coeff_0;
uniform vec4 specular_coeff_0;
uniform vec4 light_direction_0;
uniform vec4 light_pos_0;
uniform vec4 light_colour_0;

uniform vec4 emissive_coeff_1;
uniform vec4 diffuse_coeff_1;
uniform vec4 ambient_coeff_1;
uniform vec4 specular_coeff_1;
uniform vec4 light_direction_1;
uniform vec4 light_pos_1;
uniform vec4 light_colour_1;

uniform vec4 emissive_coeff_2;
uniform vec4 diffuse_coeff_2;
uniform vec4 ambient_coeff_2;
uniform vec4 specular_coeff_2;
uniform vec4 light_direction_2;
uniform vec4 light_pos_2;
uniform vec4 light_colour_2;

uniform vec4 emissive_coeff_3;
uniform vec4 diffuse_coeff_3;
uniform vec4 ambient_coeff_3;
uniform vec4 specular_coeff_3;
uniform vec4 light_direction_3;
uniform vec4 light_pos_3;
uniform vec4 light_colour_3;

uniform vec4 specular_powers;
uniform vec4 spot_powers;

vec4 directional_light(const vec4 item_colour_arg, const vec3 light_colour_arg, const vec3 light_direction_arg, const vec3 emissive_arg, const vec3 ambient_arg, const vec3 diffuse_arg, const vec3 specular_arg, float specular_power_arg)
{
	vec4 colour_results = item_colour_arg;

	vec3 emissive = emissive_arg;
	// Ambient
	vec3 ambient = ambient_arg * global_ambient.xyz;

	// Diffuse
	vec3 L = normalize(-light_direction_arg);
	float diffuse_light = max(dot(mult_normal.xyz, L), 0.0);
	vec3 diffuse = diffuse_arg * light_colour_arg * diffuse_light;

	// Specular
	vec3 V = normalize(eye_pos.xyz - v_position.xyz);
	vec3 H = normalize(L + V);

	float specular_light = pow(dot(mult_normal.xyz, H), specular_power_arg);
	if (diffuse_light <= 0.0) specular_light = 0.0;
	vec3 specular = specular_arg * light_colour_arg * specular_light;

	// Sum all light components
	vec3 light = emissive + ambient + diffuse + specular;

	// Multiply by light
	colour_results.xyz *= light;

	return colour_results;
}


vec4 point_light(const vec4 item_colour_arg, const vec3 light_colour_arg, const vec3 light_position_arg, const vec3 emissive_arg, const vec3 ambient_arg, const vec3 diffuse_arg, const vec3 specular_arg, float specular_power_arg)
{
	vec4 colour_results = item_colour_arg;

	// Emissive
	vec3 emissive = emissive_arg;

	// Ambient
	vec3 ambient = ambient_arg * global_ambient.xyz;

	// Diffuse
	vec3 L = normalize(light_position_arg - v_position.xyz);
	float diffuse_light = max(dot(mult_normal, L), 0.0);
	vec3 diffuse = diffuse_arg * light_colour_arg * diffuse_light;

	// Specular
	vec3 V = normalize(eye_pos.xyz - v_position.xyz);
	vec3 H = normalize(L + V);
	float specular_light = pow(dot(mult_normal.xyz, H), specular_power_arg);
	if (diffuse_light <= 0.0) specular_light = 0.0;
	vec3 specular = specular_arg * light_colour_arg * specular_light;

	// Sum all light components
	vec3 light = emissive + ambient + diffuse + specular;

	// Multiply by light
	colour_results.xyz *= light;

	return colour_results;
}


vec4 spot_light(const vec4 item_colour_arg, const vec3 light_colour_arg, const vec3 light_position_arg, const vec3 light_direction_arg, const vec3 emissive_arg, const vec3 ambient_arg, const vec3 diffuse_arg, const vec3 specular_arg, float specular_power_arg, float spot_power_arg)
{
	vec4 colour_results = item_colour_arg;

	// Emissive
	// vec3 emissive = emissive_arg;

	// Ambient
	vec3 ambient = ambient_arg * global_ambient.xyz;

	// Diffuse
	vec3 L = normalize(light_position_arg - v_position.xyz);
	float diffuse_light = max(dot(mult_normal, L), 0.0);
	vec3 diffuse = diffuse_arg * light_colour_arg * diffuse_light;

	// Specular
	vec3 V = normalize(eye_pos.xyz - v_position.xyz);
	vec3 H = normalize(L + V);
	float specular_light = pow(dot(mult_normal.xyz, H), specular_power_arg);
	if(diffuse_light <= 0.0) specular_light = 0.0;
	vec3 specular = specular_arg * light_colour_arg * specular_light;

	// Spot scale
	float spot_scale = pow(max(dot(L, -light_direction_arg), 0.0), spot_power_arg);

	// Sum all light components
	vec3 light = emissive_arg + ambient + (diffuse + specular) * spot_scale;

	// Multiply by light
	colour_results.xyz *= light;

	return colour_results;
}


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

	float specular_power_0 = specular_powers[0];
	float specular_power_1 = specular_powers[1];
	float specular_power_2 = specular_powers[2];
	float specular_power_3 = specular_powers[3];

	float spot_power_0 = spot_powers[0];
	float spot_power_1 = spot_powers[1];
	float spot_power_2 = spot_powers[2];	
	float spot_power_3 = spot_powers[3];

	vec4 first_pass = directional_light(tex_final, light_colour_0.xyz, light_direction_0.xyz, emissive_coeff_0.xyz, ambient_coeff_0.xyz, diffuse_coeff_0.xyz, specular_coeff_0.xyz, specular_power_0);
	vec4 second_pass = point_light(tex_final, light_colour_1.xyz, light_pos_1.xyz, emissive_coeff_1.xyz, ambient_coeff_1.xyz, diffuse_coeff_1.xyz, specular_coeff_1.xyz, specular_power_1);
	vec4 third_pass = point_light(tex_final,light_colour_2.xyz, light_pos_2.xyz, emissive_coeff_2.xyz, ambient_coeff_2.xyz, diffuse_coeff_2.xyz, specular_coeff_2.xyz, specular_power_2);
	vec4 fourth_pass = spot_light(tex_final, light_colour_3.xyz, light_pos_3.xyz, light_direction_3.xyz, emissive_coeff_3.xyz, ambient_coeff_3.xyz, diffuse_coeff_3.xyz, specular_coeff_3.xyz, specular_power_3, spot_power_3);
	vec4 output_final = (first_pass + second_pass + third_pass)/3.0;
	gl_FragColor = clamp(output_final, 0.0, 1.0);
}

// OLD
// float diffuse = clamp(dot(basic_light_0.xyz, v_normal), 0.0, 1.0);
// float diffuse = max(dot(basic_light_0.xyz, v_normal), 0.0);

// NEWLY-FOUND
/*
   vec4 directional_light(VertexShaderOutput input) : COLOR0
   {
//sample texture
vec4 color = tex2D(texSampler,input.Texture);

//Emisie
vec3 emissive = emissive_coeff;

//Ambient
vec3 ambient = ambient_coeff * global_ambient;

//Difuze
vec3 L = normalize(-light_direction);
float diffuse_light = max(dot(input.Normal,L), 0);
vec3 diffuse = diffuse_coeff*light_colour*diffuse_light;

//Specular
vec3 V = normalize(eyePosition - input.PositionO);
vec3 H = normalize(L + V);
float specular_light = pow(dot(input.Normal,H),specular_power);
if(diffuse_light<=0) specular_light=0;
vec3 specular = specular_coeff * light_colour * specular_light;

//sum all light components
vec3 light = emissive + ambient + diffuse + specular;

//multiply by light
color.rgb *= light;

return color;
}

vec4 PSPointLight(VertexShaderOutput input) : COLOR0
{
//sample texture
vec4 color = tex2D(texSampler,input.Texture);

//Emisie
vec3 emissive = emissive_coeff;

//Ambient
vec3 ambient = ambient_coeff*global_ambient;

//Difuze
vec3 L = normalize(lightPosition - input.PositionO);
float diffuse_light = max(dot(input.Normal,L), 0);
vec3 diffuse = diffuse_coeff*light_colour*diffuse_light;

//Specular
vec3 V = normalize(eyePosition - input.PositionO);
vec3 H = normalize(L + V);
float specular_light = pow(dot(input.Normal,H),specular_power);
if(diffuse_light<=0) specular_light=0;
vec3 specular = specular_coeff * light_colour * specular_light;

//sum all light components
vec3 light = emissive + ambient + diffuse + specular;

//multiply by light
color.rgb *= light;

return color;
}

vec4 PSSpotLight(VertexShaderOutput input) : COLOR0
{
//sample texture
vec4 color = tex2D(texSampler,input.Texture);

//Emisie
vec3 emissive = emissive_coeff;

//Ambient
vec3 ambient = ambient_coeff*global_ambient;

//Difuze
vec3 L = normalize(lightPosition - input.PositionO);
float diffuse_light = max(dot(input.Normal,L), 0);
vec3 diffuse = diffuse_coeff*light_colour*diffuse_light;

//Specular
vec3 V = normalize(eyePosition - input.PositionO);
vec3 H = normalize(L + V);
float specular_light = pow(dot(input.Normal,H),specular_power);
if(diffuse_light<=0) specular_light=0;
vec3 specular = specular_coeff * light_colour * specular_light;

//spot scale
float spotScale = pow(max(dot(L,-light_direction),0),spot_power);

//sum all light components
vec3 light = emissive + ambient + (diffuse + specular)*spotScale;



//multiply by light
color.rgb *= light;

return color ;
}
*/
