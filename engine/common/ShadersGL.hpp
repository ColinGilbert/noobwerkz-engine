#pragma once

namespace noob
{
	namespace glsl
	{
		////////////////////////////////////////////////////////////
		// Extremely useful for getting rid of shader conditionals
		////////////////////////////////////////////////////////////

		// http://theorangeduck.com/page/avoiding-shader-conditionals
		static const std::string shader_conditionals(
				"vec4 noob_when_eq(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return 1.0 - abs(sign(x - y));		\n"
				"}						\n"
				"vec4 noob_when_neq(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return abs(sign(x - y));		\n"
				"}						\n"
				"vec4 noob_when_gt(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return max(sign(x - y), 0.0);		\n"
				"}						\n"
				"vec4 noob_when_lt(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return min(1.0 - sign(x - y), 1.0);	\n"
				"}						\n"
				"vec4 noob_when_ge(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return 1.0 - noob_when_lt(x, y);	\n"
				"}						\n"
				"vec4 noob_when_le(vec4 x, vec4 y)		\n"
				"{						\n"
				"	return 1.0 - noob_when_gt(x, y);	\n"
				"}						\n"
				"vec4 noob_and(vec4 a, vec4 b)			\n"
				"{						\n"
				"	return a * b;				\n"
				"}						\n"
				"vec4 noob_or(vec4 a, vec4 b)			\n"
				"{						\n"
				"	return min(a + b, 1.0);			\n"
				"}						\n"
				//"vec4 xor(vec4 a, vec4 b)			\n"
				//"{						\n"
				//"	return (a + b) % 2.0;			\n"
				//"}						\n"
				"vec4 noob_not(vec4 a)				\n"
				"{						\n"
				"	return 1.0 - a;				\n"
				"}						\n"
				"float noob_when_eq(float x, float y)		\n"
				"{						\n"
				"	return 1.0 - abs(sign(x - y));		\n"
				"}						\n"
				"float noob_when_neq(float x, float y)		\n"
				"{						\n"
				"	return abs(sign(x - y));		\n"
				"}						\n"
				"float noob_when_gt(float x, float y)		\n"
				"{						\n"
				"	return max(sign(x - y), 0.0);		\n"
				"}						\n"
				"float noob_when_lt(float x, float y)		\n"
				"{						\n"
				"	return min(1.0 - sign(x - y), 1.0);	\n"
				"}						\n"
				"float noob_when_ge(float x, float y)		\n"
				"{						\n"
				"	return 1.0 - noob_when_lt(x, y);	\n"
				"}						\n"
				"float noob_when_le(float x, float y)		\n"
				"{						\n"
				"	return 1.0 - noob_when_gt(x, y);	\n"
				"}						\n");

		//////////////////////////////////
		// Goes before every shader
		//////////////////////////////////

		static const std::string shader_prefix(
				"#version 300 es		\n"
				"precision mediump float;	\n");


		//////////////////////////
		// Vertex shaders
		//////////////////////////

		static const std::string vs_instancing_src = noob::concat(shader_prefix, std::string(
					"layout(location = 0) in vec4 a_pos;				\n"
					"layout(location = 1) in vec4 a_normal;				\n"
					"layout(location = 2) in vec4 a_vert_colour;			\n"
					"layout(location = 3) in vec4 a_instance_colour;		\n"
					"layout(location = 4) in mat4 a_model_mat;			\n"
					"uniform mat4 view_matrix;					\n"
					"uniform mat4 projection_matrix;				\n"
					"out vec4 v_world_pos;						\n"
					"out vec4 v_world_normal;					\n"
					"out vec4 v_vert_colour;					\n"
					"void main()							\n"
					"{								\n"
					"	v_world_pos = a_model_mat * a_pos;			\n"
					"	mat4 modelview_mat = view_matrix * a_model_mat;		\n"
					"	v_world_normal = a_model_mat * vec4(a_normal.xyz, 0);	\n"
					"	v_vert_colour = a_vert_colour * a_instance_colour;	\n"
					"	mat4 mvp_mat = projection_matrix * modelview_mat;	\n"
					"	gl_Position = mvp_mat * a_pos;				\n"
					"}								\n"));


		static const std::string vs_terrain_src = noob::concat(shader_prefix, std::string(
					"layout(location = 0) in vec4 a_pos;				\n"
					"layout(location = 1) in vec4 a_normal;				\n"
					"layout(location = 2) in vec4 a_vert_colour;			\n"
					"out vec4 v_world_pos;						\n"
					"out vec4 v_world_normal;					\n"
					"out vec4 v_vert_colour;					\n"
					"uniform mat4 mvp;						\n"						     "uniform mat4 view_matrix;					     \n"	
					"void main()							\n"
					"{								\n"
					"	v_world_pos = a_pos;					\n"
					"	v_world_normal = vec4(a_normal.xyz, 0);			\n"
					"	v_vert_colour = a_vert_colour;				\n"
					"	gl_Position = mvp * a_pos;				\n"
					"}								\n"));

		static const std::string vs_billboard_src = noob::concat(shader_prefix, std::string(
					"layout(location = 0) in vec4 a_pos_uv;				\n"
					"layout(location = 1) in vec4 a_colour;				\n"
					"out vec2 v_uv;							\n"
					"out vec4 v_colour;						\n"
					"void main()							\n"
					"{								\n"
					"	v_uv = vec2(a_pos_uv[2], a_pos_uv[3]);			\n"
					"	v_colour = a_colour;					\n"				
					"	gl_Position = vec4(a_pos_uv[0], a_pos_uv[1], 0.0, 1.0);	\n"
					"}								\n"));



		//////////////////////////
		// Fragment shaders
		//////////////////////////

		static const std::string lambert_diffuse(
				"float lambert_diffuse(vec3 light_direction, vec3 surface_normal)	\n"
				"{									\n"
				"	return max(0.0, dot(light_direction, surface_normal));		\n"
				"}									\n");


		static const std::string blinn_phong_specular(
				"float blinn_phong_specular(vec3 light_direction, vec3 view_direction, vec3 surface_normal, float shininess)	\n"
				"{														\n"
				"	vec3 H = normalize(view_direction + light_direction);							\n"
				"	return pow(max(0.0, dot(surface_normal, H)), shininess);						\n"
				"}														\n");

		static const std::string fs_instancing_src = noob::concat(shader_prefix, lambert_diffuse, blinn_phong_specular, std::string(
					"in vec4 v_world_pos;										\n"		"in vec4 v_world_normal;										\n"
					"in vec4 v_vert_colour;										\n"
					"layout (location = 0) out vec4 out_colour;										\n"
					"uniform vec3 eye_pos;													\n"
					"uniform vec3 directional_light;											\n"	
					"void main()														\n"
					"{															\n"
					"	vec3 view_direction = normalize(eye_pos - v_world_pos.xyz);							\n"
					"	float diffuse = lambert_diffuse(-directional_light, v_world_normal.xyz);						\n"
					"	float specular = blinn_phong_specular(-directional_light, -view_direction, v_world_normal.xyz, 10.0);		\n"
					"	float light = 0.1 + diffuse + specular;										\n"
					"	out_colour = vec4(clamp(v_vert_colour.xyz * light, 0.0, 1.0), 1.0);						\n"
					"}															\n"));

		static const std::string fs_terrain_src = noob::concat(shader_prefix, lambert_diffuse, blinn_phong_specular, shader_conditionals, std::string(
					"in vec4 v_world_pos;														\n"
					"in vec4 v_world_normal;														\n"
					"in vec4 v_vert_colour;														\n"
					"layout(location = 0) out vec4 out_colour;														\n"					
					"uniform sampler2D texture_0;                        													\n"
					"uniform vec4 colour_0;																	\n"
					"uniform vec4 colour_1;																	\n"
					"uniform vec4 colour_2;																	\n"
					"uniform vec4 colour_3;																	\n"
					"uniform vec3 blend_0;																	\n"
					"uniform vec2 blend_1;																	\n"
					"uniform vec3 tex_scales;																\n"
					"uniform vec3 eye_pos;																	\n"
					"uniform vec3 directional_light;															\n"
					"void main()																		\n"
					"{																			\n"
					"	vec3 normal_blend = normalize(max(abs(v_world_normal.xyz), 0.0001));										\n"
					"	float b = normal_blend.x + normal_blend.y + normal_blend.z;											\n"
					"	normal_blend /= b;																\n"
					// "RGB-only. Try it out for great wisdom!
					// "vec4 xaxis = vec4(1.0, 0.0, 0.0, 1.0);													\n"
					// "vec4 yaxis = vec4(0.0, 1.0, 0.0, 1.0);													\n"
					// "vec4 zaxis = vec4(0.0, 0.0, 1.0, 1.0);													\n"
					"	vec4 xaxis = vec4(texture(texture_0, v_world_pos.yz * tex_scales.x).rgb, 1.0);									\n"
					"	vec4 yaxis = vec4(texture(texture_0, v_world_pos.xz * tex_scales.y).rgb, 1.0);									\n"
					"	vec4 zaxis = vec4(texture(texture_0, v_world_pos.xy * tex_scales.z).rgb, 1.0);									\n"
					"	vec4 tex = xaxis * normal_blend.x + yaxis * normal_blend.y + zaxis * normal_blend.z;								\n"
					"	float tex_r = blend_0.x * tex.r;														\n"
					"	float tex_g = blend_0.y * tex.g;														\n"
					"	float tex_b = blend_0.z * tex.b;														\n"
					"	vec4 tex_weighted = vec4(tex_r, tex_g, tex_b, 1.0);												\n"
					"	float tex_falloff = (tex_r + tex_g + tex_b) * 0.3333;												\n"
					"	float ratio_0_to_1 = noob_when_le(tex_falloff, blend_1.x) * ((tex_falloff + blend_1.x) * 0.5);							\n"
					"	float ratio_1_to_2 = noob_when_le(tex_falloff, blend_1.y) * noob_when_gt(tex_falloff, blend_1.x) * ((tex_falloff + blend_1.y) * 0.5);			\n"
					"	float ratio_2_to_3 = noob_when_ge(tex_falloff, blend_1.y) * ((tex_falloff + 1.0) * 0.5);								\n"
					"	vec4 tex_final = ((colour_0 + colour_1) * ratio_0_to_1) + ((colour_1 + colour_2) * ratio_1_to_2) + ((colour_2 + colour_3) * ratio_2_to_3);	\n"
					"	vec3 view_direction = normalize(eye_pos - v_world_pos.xyz);											\n"
					"	float diffuse = lambert_diffuse(-directional_light, v_world_normal.xyz);										\n"
					"	float specular = blinn_phong_specular(-directional_light, -view_direction, v_world_normal.xyz, 10.0);						\n"
					"	float light = 0.1 + diffuse + specular;														\n"
					"	out_colour = vec4(clamp(tex_final.xyz * light, 0.0, 1.0), 1.0);												\n"
					"}																			\n"));



		static const std::string fs_text_src = noob::concat(shader_prefix, std::string(
					"in vec2 v_uv;						\n"
					"in vec4 v_colour;						\n"
					"layout(location = 0) out vec4 out_colour;					\n"
					"uniform sampler2D texture_0;                  					\n"
					"void main()									\n"
					"{										\n"
					"	float f = texture(texture_0, v_uv).x;					\n"
					"	out_colour = vec4(v_colour.rgb, v_colour.a * f);			\n"
					"}										\n"));

		/*

		   static std::string get_light(
		   "vec3 get_light(vec3 world_pos, vec3 world_normal, vec3 eye_pos, vec3 light_pos, vec3 light_rgb)			\n"
		   "{															\n"
		   "	// vec3 normal = normalize(world_normal);									\n"
		   "	vec3 view_direction = normalize(eye_pos - world_pos);								\n"
		   "	vec3 light_direction = normalize(light_pos - world_pos);							\n"
		   "	float light_distance = length(light_pos - world_pos);								\n"
		   "	float falloff = attenuation_reid(light_pos, light_rgb, light_distance);						\n"
		   "	// Uncomment to use orenNayar, cookTorrance, etc								\n"
		   "	// float falloff = attenuation_madams(u_light_pos_r[ii].w, 0.5, light_distance);				\n"
		   "	// float roughness = u_rough_albedo_fresnel.x;									\n"
		   "	// float albedo = u_rough_albedo_fresnel.y;									\n"
		   "	// float fresnel = u_rough_albedo_fresnel.z;									\n"
		   "	float diffuse_coeff = lambert_diffuse(light_direction, world_normal);						\n"
		   "	// float diffuse_coeff = oren+nayar_diffuse(light_direction, view_direction, world_normal, roughness, albedo);	\n"
		   "	float diffuse = (diffuse_coeff * falloff);									\n"
		   "	// float specular = 0.0;											\n"
		   "	float specular = blinn_phong_specular(light_direction, view_direction, world_normal, u_shine);			\n"
		   "	// float specular = cook_torrance_specular(light_direction, view_direction, world_normal, roughness, fresnel);	\n"
		   "	vec3 light_colour = (diffuse + specular) * light_rgb;								\n"
		   "	return light_colour;												\n"
		   "}															\n");

		   static std::string vs_texture_2d_src(
		   "#version 300 es                            \n"
		   "layout(location = 0) in vec4 a_position;   \n"
		   "layout(location = 1) in vec2 a_texCoord;   \n"
		   "out vec2 v_texCoord;                       \n"
		   "void main()                                \n"
		   "{                                          \n"
		   "   gl_Position = a_position;               \n"
		   "   v_texCoord = a_texCoord;                \n"
		   "}                                          \n");

		   static std::string fs_texture_2d_src(
		   "#version 300 es                                     \n"
		   "precision mediump float;                            \n"
		   "in vec2 v_texCoord;                                 \n"
		   "layout(location = 0) out vec4 outColor;             \n"
		   "uniform sampler2D s_texture;                        \n"
		   "void main()                                         \n"
		   "{                                                   \n"
		   "  outColor = texture( s_texture, v_texCoord );      \n"
		   "}                                                   \n");

		   static std::string vs_cubemap_src(
		   "#version 300 es                            \n"
		   "layout(location = 0) in vec4 a_position;   \n"
		   "layout(location = 1) in vec3 a_normal;     \n"
		   "out vec3 v_normal;                         \n"
		   "void main()                                \n"
		   "{                                          \n"
		   "   gl_Position = a_position;               \n"
		   "   v_normal = a_normal;                    \n"
		   "}                                          \n");

		   static std::string fs_cubemap_src(
		   "#version 300 es                                     \n"
		   "precision mediump float;                            \n"
		   "in vec3 v_normal;                                   \n"
		   "layout(location = 0) out vec4 outColor;             \n"
		   "uniform samplerCube s_texture;                      \n"
		   "void main()                                         \n"
		   "{                                                   \n"
		   "   outColor = texture(s_texture, v_normal);         \n"
		   "}                                                   \n");
		   */
	}
}
