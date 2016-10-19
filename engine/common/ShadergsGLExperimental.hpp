#pragma once

namespace noob
{
	namespace glsl
	{
		static const std::string version() noexcept(true)
		{
			return "#version 300 es\n";
		}

		static const std::string lowp() noexcept(true)
		{
			return "precision lowp float;\n";
		}

		static const std::string mediump() noexcept(true)
		{
			return "precision mediump float;\n";
		}

		static const std::string highp() noexcept(true)
		{
			return "precision highp float;\n";
		}

		static std::string vs_position_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 a_position;\n");
		}

		static std::string vs_colour_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 a_normal;\n");
		}

		static std::string vs_normal_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 a_colour;\n");
		}

		static std::string vs_texcoord_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 a_texcoords;\n");
		}

		static std::string vs_indices_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in ivec4 a_indices;\n");
		}

		static std::string vs_matrix_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in mat4 a_matrix;\n");
		}

		static std::string vs_colour_output(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") out vec4 v_color\n");
		}

		static std::string vs_normal_output(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") out vec4 v_normal;\n");
		}

		static std::string vs_texcoord_output(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") out vec4 v_texcoords;\n");

		}

		static std::string fs_colour_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 v_colour;\n");

		}

		static std::string fs_colour_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 v_colour;\n");
		}

		static std::string fs_normal_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 v_normal;\n");
		}

		static std::string fs_texcoord_input(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") in vec4 v_texcoords;\n");
		}

		static std::string fs_colour_output(uint32_t layout) noexcept(true)
		{
			return noob::concat("layout(location = ", noob::to_string(layout), ") out vec4 out_color;\n");
		}

		static std::string sampler_2d() noexcept(true)
		{
			return "uniform sampler2DArray sampler_2d;\n";
		}

		static std::string open_block() noexcept(true)
		{
			return "\n{\n";
		}

		static std::string end_block() noexcept(true)
		{
			return "\n};\n";
		}

		static std::string end_func() noexcept(true)
		{
			return "\n}\n";
		}

		static std::string begin_main() noexcept(true)
		{
			return noob::concat("void main()", open_block());
		}
	};
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
