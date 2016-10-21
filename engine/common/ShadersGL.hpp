#pragma once

namespace noob
{
	namespace glsl
	{

		static std::string vs_instancing_src(
				"#version 300 es                                \n"
				"precision mediump float;                       \n"
				"layout(location = 0) in vec4 a_pos;            \n"
				"layout(location = 1) in vec4 a_normal;         \n"
				"layout(location = 2) in vec4 a_colour;         \n"
				"layout(location = 3) in vec4 a_colour_mult;    \n"
				"layout(location = 4) in mat4 a_mvp;            \n"
				"layout(location = 0) out vec4 v_normal;        \n"
				"layout(location = 1) out vec4 v_colour;        \n"
				"void main()                                    \n"
				"{                                              \n"
				"   v_normal = a_normal;                        \n"
				"   v_colour = a_colour * a_colour_mult;        \n"
				"   gl_Position = a_mvp * a_position;           \n"
				"}                                              \n");


		static std::string fs_instancing_src(
				"#version 300 es                                \n"
				"precision mediump float;                       \n"
				"layout(location = 0) in vec4 v_normal;         \n"
				"layout(location = 1) in vec4 v_colour;         \n"
				"layout (location = 0) out vec4 out_colour;     \n"
				"void main()                                    \n"
				"{                                              \n"
				"  out_colour = v_colour;                       \n"
				"}                                              \n");

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
	}
}
