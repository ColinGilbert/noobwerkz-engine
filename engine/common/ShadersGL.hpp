#pragma once
#include <string>
namespace noob
{
	namespace glsl
	{
		static const std::string version = "#version 300 es\n";
		static const std::string lowp = "precision lowp float;\n";
		static const std::string mediump = "precision mediump float;\n";
		static const std::string highp = "precision highp float;\n";
		static const std::string vs_position = "layout(location = 0) in vec4 a_position;\n";
		static const std::string vs_colour = "layout(location = 1) in vec4 a_normal;\n";
		static const std::string vs_normal = "layout(location = 2) in vec4 a_colour;\n";
		static const std::string vs_texcoord = "layout(location = 3) in vec4 a_texcoords;\n";
      		static const std::string vs_matrix = "layout(location = 4) in mat4 a_matrix;\n";
		static const std::string vs_output = "out vec4 v_color;\n";
		static const std::string end_func = "\n}\n";
		static const std::string end_block = "\n};\n";
		static const std::string begin_main = "void main()\n{\n";
	}

	static std::string fs_instancing_src(
			"#version 300 es                                \n"
			"precision mediump float;                       \n"
			"in vec4 v_color;                               \n"
			"layout(location = 0) out vec4 outColor;        \n"
			"void main()                                    \n"
			"{                                              \n"
			"  outColor = v_color;                          \n"
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
