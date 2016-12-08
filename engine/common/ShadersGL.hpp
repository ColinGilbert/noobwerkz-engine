#pragma once

namespace noob
{
	namespace glsl
	{
		static const std::string shader_prefix(
				"#version 300 es		\n"
				"precision mediump float;	\n");


		static const std::string vs_instancing_src = noob::concat(shader_prefix, std::string(
				"layout(location = 0) in vec4 a_pos;				\n"
				"layout(location = 1) in vec4 a_normal;				\n"
				"layout(location = 2) in vec4 a_vertex_colour;			\n"
				"layout(location = 3) in vec4 a_instance_colour;		\n"
				"layout(location = 4) in mat4 a_model_mat;			\n"
				"layout(location = 8) in mat4 a_mvp_mat;			\n"
				"layout(location = 0) out vec4 world_pos;			\n"
				"layout(location = 1) out vec4 world_normal;			\n"
				"layout(location = 2) out vec4 vert_colour;			\n"				
				"void main()							\n"
				"{								\n"
				"	world_pos = a_model_mat * a_pos;			\n"
				"	world_normal = a_model_mat * a_normal;			\n"				
				"	vert_colour = a_vertex_colour * a_instance_colour;	\n"
				"	gl_Position = a_mvp_mat * a_pos;			\n"
				"}								\n"));


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

	
		static const std::string fs_instancing_partial(
				"layout(location = 0) in vec4 world_pos;										\n"			
				"layout(location = 1) in vec4 world_normal;										\n"
				"layout(location = 2) in vec4 vert_colour;										\n"
				"layout(location = 0) out vec4 out_colour;										\n"
				"uniform vec3 eye_pos;													\n"
				"uniform vec3 directional_light;											\n"	
				"void main()														\n"
				"{															\n"
				"	vec3 view_direction = normalize(eye_pos - world_pos.xyz);							\n"
				"	float diffuse = lambert_diffuse(directional_light, world_normal.xyz);						\n"
				"	float specular = blinn_phong_specular(directional_light.xyz, view_direction, world_normal.xyz, 10.0);		\n"
				"	float light = 0.1 + diffuse + specular;										\n"
				"	out_colour = vec4(clamp(light * vert_colour.xyz, 0.0, 1.0), 1.0);						\n"
				"}															\n");


		static const std::string fs_instancing_src = noob::concat(shader_prefix, lambert_diffuse, blinn_phong_specular, fs_instancing_partial);


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
