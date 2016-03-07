$input v_position, v_normal, world_pos, world_normal, world_eye

#include "common.sh"

SAMPLER2D(texture_0, 0);

// Individual colours associated with the gradient 0.0-1.0.
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

uniform mat4 normal_mat;

// Lighting
uniform vec4 eye_pos;
uniform vec4 eye_pos_normalized;

uniform vec4 u_ambient;
uniform vec4 u_diffuse;
uniform vec4 u_specular_shine;
uniform vec4 u_fog;
uniform vec4 u_rough_albedo_fresnel;

#define MAX_LIGHTS 6
const float MAX_LIGHTS_F = 6.0;

uniform vec4 u_light_rgb_intensity[MAX_LIGHTS];
uniform vec4 u_light_pos_r[MAX_LIGHTS];

#define PI 3.14159265

#define u_specular      u_specular_shine.xyz
#define u_shine     u_specular_shine.w


float beckmannDistribution(float x, float roughness)
{
	float NdotH = max(x, 0.0001);
	float cos2Alpha = NdotH * NdotH;
	float tan2Alpha = (cos2Alpha - 1.0) / cos2Alpha;
	float roughness2 = roughness * roughness;
	float denom = 3.141592653589793 * roughness2 * cos2Alpha * cos2Alpha;
	return exp(tan2Alpha / roughness2) / denom;
}


float attenuation_reid(float r, float f, float d)
{
	return pow(max(0.0, 1.0 - (d / r)), f + 1.0);
}


// TODO: To avoid possibility of divide by zero, r > 0 and f != 1.0
float attenuation_madams(float r, float f, float d)
{
	float denom = d / r + 1.0;
	float attenuation = 1.0 / (denom*denom);
	float t = (attenuation - f) / (1.0 - f);
	return max(t, 0.0);
}


float lambertDiffuse(vec3 lightDirection, vec3 surfaceNormal)
{
	return max(0.0, dot(lightDirection, surfaceNormal));
}


float orenNayarDiffuse(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float albedo)
{

	float LdotV = dot(lightDirection, viewDirection);
	float NdotL = dot(lightDirection, surfaceNormal);
	float NdotV = dot(surfaceNormal, viewDirection);

	float s = LdotV - NdotL * NdotV;
	float t = mix(1.0, max(NdotL, NdotV), step(0.0, s));

	float sigma2 = roughness * roughness;
	float A = 1.0 + sigma2 * (albedo / (sigma2 + 0.13) + 0.5 / (sigma2 + 0.33));
	float B = 0.45 * sigma2 / (sigma2 + 0.09);

	return albedo * max(0.0, NdotL) * (A + B * s / t) / PI;
}


float blinnPhongSpecular(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float shininess)
{
	//Calculate Blinn-Phong power
	vec3 H = normalize(viewDirection + lightDirection);
	return pow(max(0.0, dot(surfaceNormal, H)), shininess);
}


float beckmannSpecular(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness)
{
	return beckmannDistribution(dot(surfaceNormal, normalize(lightDirection + viewDirection)), roughness);
}


float wardSpecular(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, vec3 fiberParallel, vec3 fiberPerpendicular, float shinyParallel, float shinyPerpendicular)
{

	float NdotL = dot(surfaceNormal, lightDirection);
	float NdotR = dot(surfaceNormal, viewDirection);

	if(NdotL < 0.0 || NdotR < 0.0) 
	{
		return 0.0;
	}

	vec3 H = normalize(lightDirection + viewDirection);

	float NdotH = dot(surfaceNormal, H);
	float XdotH = dot(fiberParallel, H);
	float YdotH = dot(fiberPerpendicular, H);

	float coeff = sqrt(NdotL/NdotR) / (4.0 * PI * shinyParallel * shinyPerpendicular); 
	float theta = (pow(XdotH/shinyParallel, 2.0) + pow(YdotH/shinyPerpendicular, 2.0)) / (1.0 + NdotH);

	return coeff * exp(-2.0 * theta);
}


float cookTorranceSpecular(vec3 lightDirection, vec3 viewDirection, vec3 surfaceNormal, float roughness, float fresnel)
{
	float VdotN = max(dot(viewDirection, surfaceNormal), 0.0);
	float LdotN = max(dot(lightDirection, surfaceNormal), 0.0);

	//Half angle vector
	vec3 H = normalize(lightDirection + viewDirection);

	//Geometric term
	float NdotH = max(dot(surfaceNormal, H), 0.0);
	float VdotH = max(dot(viewDirection, H), 0.000001);
	float LdotH = max(dot(lightDirection, H), 0.000001);
	float G1 = (2.0 * NdotH * VdotN) / VdotH;
	float G2 = (2.0 * NdotH * LdotN) / LdotH;
	float G = min(1.0, min(G1, G2));

	//Distribution term
	float D = beckmannDistribution(NdotH, roughness);

	//Fresnel term
	float F = pow(1.0 - VdotN, fresnel);

	//Multiply terms and done
	return  G * F * D / max(3.14159265 * VdotN, 0.000001);
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

	vec3 normal = normalize(world_normal);
	vec3 view_direction = normalize(world_eye - world_pos);

	vec3 total_light = vec3(0.0, 0.0, 0.0);

	for (int ii = 0; ii < MAX_LIGHTS; ++ii)
	{
		vec3 light_direction = normalize(u_light_pos_r[ii].xyz - world_pos);
		float light_distance = length(u_light_pos_r[ii].xyz - world_pos);

		float falloff = attenuation_reid(u_light_pos_r[ii].w, 0.5, light_distance);
		// float falloff = attenuation_madams(u_light_pos_r[ii].w, 0.5, light_distance);

		// Uncomment to use orenNayar, cookTorrance, etc...
		// float roughness = u_rough_albedo_fresnel.x;
		// float albedo = u_rough_albedo_fresnel.y;
		// float fresnel = u_rough_albedo_fresnel.z;

		float diffuse_coeff = lambertDiffuse(light_direction, normal);
		//float diffuse_coeff = orenNayarDiffuse(light_direction, view_direction, normal, roughness, albedo);
		float diffuse = (diffuse_coeff * falloff);// * u_light_rgb_intensity[ii].rgb;

		float specular = blinnPhongSpecular(light_direction, view_direction, normal, u_shine);
		// float specular = cookTorranceSpecular(light_direction, view_direction, normal, roughness, fresnel);

		vec3 light_colour = (diffuse + specular) * u_light_rgb_intensity[ii].rgb; // * u_light_rgb_intensity[ii].a;
		total_light += light_colour;
	}

	total_light *= (1.0 / MAX_LIGHTS_F);
	vec3 total_colour = (tex_final.xyz * u_ambient.xyz) + total_light;
	
	gl_FragColor.xyz = clamp(total_colour, 0.0, 1.0);
	gl_FragColor.w = tex_final.w;
}
