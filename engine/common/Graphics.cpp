// Header files we're implementing
#include "Graphics.hpp"

// std
#include <algorithm>

// External
#include <GLES3/gl3.h>

// Project-local
#include "ShadersGL.hpp"
#include "NoobUtils.hpp"
#include "StringFuncs.hpp"

//////////////////////////////////////////////////////////////
// Error-checking helper + macro. Good for troubleshooting.
/////////////////////////////////////////////////////////////

static GLenum check_error_gl(const char *file, int line)
{
	GLenum error_code;
	while ((error_code = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (error_code)
		{
			case GL_INVALID_ENUM:
				error = "INVALID_ENUM";
				break;
			case GL_INVALID_VALUE:              
				error = "INVALID_VALUE"; 
				break;
			case GL_INVALID_OPERATION:            
				error = "INVALID_OPERATION";
				break;
			case GL_OUT_OF_MEMORY:
				error = "OUT_OF_MEMORY"; 
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: 
				error = "INVALID_FRAMEBUFFER_OPERATION";
				break;
		}
		noob::logger::log(noob::importance::ERROR, noob::concat("OpenGL: ", error, " ", file, " (", noob::to_string(line), ")"));
	}
	return error_code;
}


#define check_error_gl() check_error_gl(__FILE__, __LINE__) 

///////////////////////////////////////////////////////////
// This is where we place several other helper functions
///////////////////////////////////////////////////////////


///////////////////////////////////////////////
// Various useful formats supported by GLES3:
///////////////////////////////////////////////

// Uncompressed formats, per-channel:
// GL_UNSIGNED_BYTE
// GL_BYTE
// GL_UNSIGNED_SHORT
// GL_SHORT
// GL_UNSIGNED_INT
// GL_INT
// GL_HALF_FLOAT
// GL_FLOAT

// Uncompressed formats, per-pixel:
// GL_UNSIGNED_SHORT_5_6_5
// GL_UNSIGNED_SHORT_4_4_4_4
// GL_UNSIGNED_SHORT_5_5_5_1
// GL_UNSIGNED_INT_2_10_10_10_REV
// GL_UNSIGNED_INT_10F_11F_11F_REV
// GL_UNSIGNED_INT_5_9_9_9_REV
// GL_UNSIGNED_INT_24_8
// GL_FLOAT_32_UNSIGNED_INT_24_8_REV.

// Compressed formats (per pixel):
// GL_COMPRESSED_R11_EAC,
// GL_COMPRESSED_SIGNED_R11_EAC,
// GL_COMPRESSED_RG11_EAC,
// GL_COMPRESSED_SIGNED_RG11_EAC,
// GL_COMPRESSED_RGB8_ETC2,
// GL_COMPRESSED_SRGB8_ETC2,
// GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
// GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
// GL_COMPRESSED_RGBA8_ETC2_EAC, or
// GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC


static GLenum get_pixel_format_sized(const noob::pixel_format Pixels)
{
	GLenum results;

	switch(Pixels)
	{
		case(noob::pixel_format::R8):
			{
				results = GL_R8;
				break;
			}
		case(noob::pixel_format::RG8):
			{
				results = GL_RG8;
				break;				
			}
		case(noob::pixel_format::RGB8):
			{
				results = GL_RGB8;
				break;				
			}
		case(noob::pixel_format::SRGB8):
			{
				results = GL_SRGB8;
				break;				
			}
		case(noob::pixel_format::RGBA8):
			{
				results = GL_RGBA8;
				break;
			}
		case(noob::pixel_format::SRGBA8):
			{
				results = GL_SRGB8_ALPHA8;		
				break;

			}
		case(noob::pixel_format::RGB8_COMPRESSED):
			{
				results = GL_COMPRESSED_RGB8_ETC2;		
				break;

			}
		case(noob::pixel_format::SRGB8_COMPRESSED):
			{
				results = GL_COMPRESSED_SRGB8_ETC2;		
				break;

			}
		case(noob::pixel_format::RGB8_A1_COMPRESSED):
			{
				results = GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;	
				break;

			}
		case(noob::pixel_format::SRGB8_A1_COMPRESSED):
			{
				results = GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
				break;

			}
		case(noob::pixel_format::RGBA8_COMPRESSED):
			{
				results = GL_COMPRESSED_RGBA8_ETC2_EAC;
				break;

			}
		case(noob::pixel_format::SRGBA8_COMPRESSED):
			{		
				results = GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
				break;

			}
	}

	return results;
}


// This one gives you the "format" and "type" arguments for glTexSubImage2D and glTexSubImage3D
// Visit: https://www.khronos.org/opengles/sdk/docs/man3/html/glTexSubImage2D.xhtml and https://www.khronos.org/opengles/sdk/docs/man3/html/glTexSubImage3D.xhtml
// Uou see, OpenGL doesn't simply allow you to use the buffer's storage format as the argument to the the functions that are used to fill the buffer, as that would be far too straightforward.
// Instead, we must write otherwise pointless functions such as these to make our dreams into reality. Here is me rooting for Vulkan,
static GLenum get_pixel_format_unsized(noob::pixel_format Pixels)
{
	GLenum results;

	switch(Pixels)
	{
		case(noob::pixel_format::R8):
			{
				results =  GL_RED;
				break;
			}
		case(noob::pixel_format::RG8):
			{
				results = GL_RG;
				break;				
			}
		case(noob::pixel_format::RGB8):
		case(noob::pixel_format::SRGB8):
			{
				results = GL_RGB;		
				break;			
			}
		case(noob::pixel_format::RGBA8):
		case(noob::pixel_format::SRGBA8):
			{
				results = GL_RGBA;		
				break;
			}
		default:
			{
				noob::logger::log(noob::importance::WARNING, "Invalid value hit while deducing unsized pixel format!");
				break;
			}
	}
	return results;
}


static GLenum get_wrapping(noob::tex_wrap_mode WrapMode)
{
	GLenum results;
	switch (WrapMode)
	{
		case(noob::tex_wrap_mode::CLAMP_TO_EDGE):
			{
				results = GL_CLAMP_TO_EDGE;
				break;
			}
		case(noob::tex_wrap_mode::MIRRORED_REPEAT):
			{
				results = GL_MIRRORED_REPEAT;
				break;
			}
		case(noob::tex_wrap_mode::REPEAT):
			{
				results = GL_REPEAT;
				break;
			}
	};
	return results;
}


static GLenum get_swizzle(noob::tex_swizzle Swizzle)
{
	GLenum results;
	switch (Swizzle)
	{
		case(noob::tex_swizzle::RED):
			{
				results = GL_RED;
				break;
			}
		case(noob::tex_swizzle::GREEN):
			{
				results = GL_GREEN;
				break;
			}
		case(noob::tex_swizzle::BLUE):
			{
				results = GL_BLUE;
				break;
			}
		case(noob::tex_swizzle::ALPHA):
			{
				results = GL_ALPHA;
				break;
			}
		case(noob::tex_swizzle::ZERO):
			{
				results = GL_ZERO;
				break;
			}
		case(noob::tex_swizzle::ONE):
			{
				results = GL_ONE;
				break;
			}
	};
	return results;
}


static GLuint load_shader_gl(GLenum type, const std::string& shader_arg)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
	{
		return 0;
	}

	const char* shader_src = shader_arg.c_str();
	// Load the shader source
	glShaderSource(shader, 1, &shader_src, NULL);
	check_error_gl();

	// Compile the shader
	glCompileShader(shader);
	check_error_gl();

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	check_error_gl();

	if (!compiled)
	{
		GLint info_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);
		check_error_gl();

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetShaderInfoLog(shader, info_len, NULL, &info_log[0]);
			check_error_gl();

			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error compiling shader: ", info_log));
		}

		glDeleteShader(shader);
		check_error_gl();

		return 0;
	}

	return shader;
}


static GLuint load_program_gl(const std::string& vert_shader_arg, const std::string frag_shader_arg)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program_object;
	GLint linked;

	// Just to make sure no dirty state gets to ruin our shader compile. :)
	glBindVertexArray(0);

	const char* vert_shader_src = vert_shader_arg.c_str();
	// Load the vertex/fragment shaders
	vertex_shader = load_shader_gl(GL_VERTEX_SHADER, vert_shader_arg);
	check_error_gl();

	if (vertex_shader == 0)
	{
		return 0;
	}

	const char* frag_shader_src  = frag_shader_arg.c_str();
	fragment_shader = load_shader_gl(GL_FRAGMENT_SHADER, frag_shader_arg);
	check_error_gl();

	if (fragment_shader == 0)
	{
		glDeleteShader(vertex_shader);
		return 0;
	}

	// Create the program object
	program_object = glCreateProgram();
	check_error_gl();

	if (program_object == 0)
	{
		return 0;
	}

	glAttachShader(program_object, vertex_shader);
	check_error_gl();

	glAttachShader(program_object, fragment_shader);
	check_error_gl();

	// Link the program
	glLinkProgram(program_object);
	check_error_gl();

	// Check the link status
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);
	check_error_gl();

	if (!linked)
	{
		GLint info_len = 0;

		glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);
		check_error_gl();

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetProgramInfoLog(program_object, info_len, NULL, &info_log[0]);
			check_error_gl();

			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error linking program:", info_log));

		}

		glDeleteProgram(program_object);
		return 0;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertex_shader);
	check_error_gl();
	glDeleteShader(fragment_shader);
	check_error_gl();

	return program_object;
}

// This allows us to return a texture_id that can be made const by the programmer.
static GLuint prep_texture()
{
	GLuint texture_id;

	glGenTextures(1, &texture_id);

	return texture_id;
}


// File-local function that helps reduce repeated code.
static bool texture_packing_valid(uint32_t Arg) noexcept(true)
{
	// GL_PACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8
	switch (Arg)
	{
		case (1):
		case (2):
		case (4):
		case (8):
			{
				return true;

			}
	};

	return false;

}

/////////////////////////////////////////////////////////////////////////
// Finally, this is where the implementation of the interface begins.
/////////////////////////////////////////////////////////////////////////


void noob::graphics::init(const noob::vec2ui Dims, const noob::texture_loader_2d& TexLoader) noexcept(true)
{
	instanced_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_instancing_src, noob::glsl::fs_instancing_src));
	check_error_gl();
	u_eye_pos = glGetUniformLocation(instanced_shader.index(), "eye_pos");
	check_error_gl();	
	u_light_directional = glGetUniformLocation(instanced_shader.index(), "directional_light");
	check_error_gl();

	noob::logger::log(noob::importance::INFO, "Loading terrain shader!");

	terrain_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_terrain_src, noob::glsl::fs_terrain_src));
	check_error_gl();

	// Init terrain
	max_terrain_verts = 4096;

	terrain_unis.texture_weights = noob::vec3f(0.3, 0.3, 0.4);
	terrain_unis.colours[0] = noob::vec4f(0.0, 0.5, 0.4, 1.0);
	terrain_unis.colours[1] = noob::vec4f(0.1, 0.5, 0.5, 1.0);
	terrain_unis.colours[2] = noob::vec4f(0.5, 0.5, 0.5, 1.0);
	terrain_unis.colours[3] = noob::vec4f(1.0, 1.0, 1.0, 1.0);	
	terrain_unis.colour_offsets = noob::vec2f(0.6, 0.8);
	terrain_unis.texture_scales = noob::vec3f(0.5, 0.5, 0.5);

	u_mvp_terrain = glGetUniformLocation(terrain_shader.index(), "mvp");
	u_texture_0 = glGetUniformLocation(terrain_shader.index(), "texture_0");
	u_colour_0 = glGetUniformLocation(terrain_shader.index(), "colour_0");
	u_colour_1 = glGetUniformLocation(terrain_shader.index(), "colour_1");
	u_colour_2 = glGetUniformLocation(terrain_shader.index(), "colour_2");
	u_colour_3 = glGetUniformLocation(terrain_shader.index(), "colour_3");
	u_blend_0 = glGetUniformLocation(terrain_shader.index(), "blend_0");
	u_blend_1 = glGetUniformLocation(terrain_shader.index(), "blend_1");
	u_tex_scales = glGetUniformLocation(terrain_shader.index(), "tex_scales");
	u_eye_pos_terrain = glGetUniformLocation(terrain_shader.index(), "eye_pos");
	u_light_directional_terrain = glGetUniformLocation(terrain_shader.index(), "directional_light");
	check_error_gl();

	noob::texture_info texinfo;
	terrain_tex = texture_2d(TexLoader, true, false);
	check_error_gl();

	texture_wrap_mode(std::array<noob::tex_wrap_mode, 2>({tex_wrap_mode::REPEAT, tex_wrap_mode::REPEAT}));
	texture_mag_filter(noob::tex_mag_filter::NEAREST);
	texture_min_filter(noob::tex_min_filter::NEAREST);
	check_error_gl();

	glGenerateMipmap(GL_TEXTURE_2D);
	check_error_gl();

	glGenVertexArrays(1, &terrain.vao);
	glBindVertexArray(terrain.vao);
	glGenBuffers(1, &terrain.vertices_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, terrain.vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_terrain_verts * noob::terrain_model::stride, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, noob::terrain_model::stride, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, noob::terrain_model::stride, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, noob::terrain_model::stride, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	check_error_gl();


	// Init text

	text_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_billboard_src, noob::glsl::fs_text_src));
	u_text_texture_0 = glGetUniformLocation(text_shader.index(), "texture_0");
	check_error_gl();



	glBindVertexArray(0);

	// Do an initial frame and log
	frame(Dims);
	noob::logger::log(noob::importance::INFO, "[Graphics] Done init.");
}


void noob::graphics::frame(const noob::vec2ui Dims) const noexcept(true)
{
	glBindVertexArray(0);
	glViewport(0, 0, Dims[0], Dims[1]);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glClearColor(0.2f, 0.0f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	check_error_gl();
}


void noob::graphics::destroy() noexcept(true)
{

}


void noob::graphics::use_program(noob::graphics::program_handle Arg) const noexcept(true)
{
	glUseProgram(Arg.index());
	check_error_gl();
}


void noob::graphics::set_view_mat(const noob::mat4f ViewMat) noexcept(true)
{
	eye_pos = noob::translation_from_mat4(ViewMat);
	view_mat = ViewMat;
}


void noob::graphics::set_projection_mat(const noob::mat4f ProjMat) noexcept(true)
{
	proj_mat = ProjMat;
}


void noob::graphics::upload_instanced_uniforms() const noexcept(true)
{
	glUniform3fv(u_eye_pos_terrain, 1, &eye_pos[0]);
	glUniform3fv(u_light_directional_terrain, 1, &light_direction[0]);
	check_error_gl();
}


void noob::graphics::draw_instanced(const noob::instanced_model_handle Handle, uint32_t NumInstances) const noexcept(true)
{
	const noob::instanced_model m = instanced_models[Handle.index()];

	glBindVertexArray(m.vao);
	glDrawElementsInstanced(GL_TRIANGLES, m.n_indices, GL_UNSIGNED_INT, reinterpret_cast<const void *>(0), std::min(m.n_instances, NumInstances));
	check_error_gl();
	glBindVertexArray(0);		
}



void noob::graphics::draw_text(const noob::billboard_buffer_handle Handle, noob::texture_2d_handle Tex, uint32_t NumBillboards) const noexcept(true)
{
	const noob::billboard_buffer bb = billboards[Handle.index()];

	use_program(text_shader);
	glActiveTexture(GL_TEXTURE0);
	glUniform1i(u_text_texture_0, 0);

	glBindVertexArray(bb.vao);

	bind_texture(Tex);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	//glDepthFunc(GL_NEVER);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	check_error_gl();

	glDrawArrays(GL_TRIANGLES, 0, NumBillboards * 6);
	check_error_gl();

	// glDisable(GL_BLEND);
	// glEnable(GL_CULL_FACE);
	// glEnable(GL_DEPTH_TEST);

	glBindVertexArray(0);		
}





void noob::graphics::draw_terrain(uint32_t Verts) const noexcept(true)
{
	glBindVertexArray(terrain.vao);
	use_program(terrain_shader);
	glActiveTexture(GL_TEXTURE0);
	check_error_gl();
	bind_texture(terrain_tex);
	upload_terrain_uniforms();
	glDrawArrays(GL_TRIANGLES, 0, Verts);
	check_error_gl();
	glBindVertexArray(0);
}


noob::instanced_model_handle noob::graphics::add_instanced_models(const noob::mesh_3d& Mesh, uint32_t MaxInstances) noexcept(true)
{
	if (MaxInstances == 0)
	{
		return noob::instanced_model_handle::make_invalid();
	}
	noob::instanced_model model;

	model.n_instances = MaxInstances;

	const uint32_t num_indices = Mesh.indices.size();
	model.n_indices = num_indices;

	const uint32_t num_verts = Mesh.vertices.size();
	model.n_vertices = num_verts;

	GLuint vao_id = 0;
	glGenVertexArrays(1, &vao_id);
	glBindVertexArray(vao_id);
	model.vao = vao_id;

	////////////////////////////////
	// Create & bind attrib buffers
	////////////////////////////////

	std::array<GLuint, 4> vbo_ids;
	glGenBuffers(4, &vbo_ids[0]);

	const GLuint vertices_vbo = vbo_ids[0];
	const GLuint colours_vbo = vbo_ids[1];
	const GLuint matrices_vbo = vbo_ids[2];
	const GLuint indices_vbo = vbo_ids[3];

	/////////////////////////////////////////////////
	// Setup non-instanced, interleaved attribs VBO:
	/////////////////////////////////////////////////

	std::vector<noob::vec4f> interleaved;
	interleaved.resize(num_verts * 3);

	// Interleave our vertex positions, normals, and colours
	for(uint32_t i = 0; i < num_verts; ++i)
	{
		const uint32_t current_offset = i * 3;
		interleaved[current_offset] = noob::vec4f(Mesh.vertices[i].v[0], Mesh.vertices[i].v[1], Mesh.vertices[i].v[2], 1.0);
		interleaved[current_offset + 1] = noob::vec4f(Mesh.normals[i].v[0], Mesh.normals[i].v[1], Mesh.normals[i].v[2], 0.0);
		interleaved[current_offset + 2] = Mesh.colours[i];
	}

	// Upload interleaved buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(noob::vec4f), &interleaved[0], GL_STATIC_DRAW);
	model.vertices_vbo = vertices_vbo;

	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(0));
	// Normals
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	// Vertex colours
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f)*3, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Setup colours VBO:
	// std::vector<noob::vec4f> colours(MaxInstances, noob::vec4f(1.0, 1.0, 1.0, 1.0));
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, MaxInstances * noob::instanced_model::colours_stride, nullptr, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, MaxInstances * noob::model::materials_stride, &colours[0].v[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4f), reinterpret_cast<const void *>(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	model.colours_vbo = colours_vbo;

	// Setup matrices VBO:
	// std::vector<noob::mat4f> matrices(MaxInstances * 2, noob::identity_mat4());
	glBindBuffer(GL_ARRAY_BUFFER, matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, MaxInstances * noob::instanced_model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, MaxInstances * noob::model::matrices_stride, &matrices[0].m[0], GL_DYNAMIC_DRAW);

	// Per instance model matrices
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*2));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*3));
	// Per-instance MVP matrices
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*4));
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*5));
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*6));
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4f)*2, reinterpret_cast<const void *>(sizeof(noob::vec4f)*7));

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glEnableVertexAttribArray(9);
	glEnableVertexAttribArray(10);
	glEnableVertexAttribArray(11);

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);

	model.matrices_vbo = matrices_vbo;

	// Upload to indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, Mesh.indices.size() * sizeof(uint32_t), &Mesh.indices[0], GL_STATIC_DRAW);
	model.indices_vbo = indices_vbo;

	glBindVertexArray(0);

	check_error_gl();

	instanced_models.push_back(model);

	const noob::bbox_type<float> bb = Mesh.bbox;

	const noob::instanced_model_handle results = noob::instanced_model_handle::make(instanced_models.size() - 1);

	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created instanced model with handle ", noob::to_string(results.index()), " and ", noob::to_string(MaxInstances), " instances. Verts = ", noob::to_string(Mesh.vertices.size()), ", indices = ", noob::to_string(Mesh.indices.size()), ". Dims: ", noob::to_string(bb.max - bb.min)));

	return results;
}


// TODO
noob::billboard_buffer_handle noob::graphics::add_billboards(uint32_t MaxBillboards) noexcept(true)
{
	if (MaxBillboards == 0)
	{
		return noob::billboard_buffer_handle::make_invalid();
	}

	glBindVertexArray(0);

	noob::billboard_buffer results;

	results.max = MaxBillboards;

	glGenVertexArrays(1, &results.vao);
	glBindVertexArray(results.vao);

	glGenBuffers(1, &results.vertices_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, results.vertices_vbo);

	glBufferData(GL_ARRAY_BUFFER, MaxBillboards * noob::billboard_buffer::stride, nullptr, GL_DYNAMIC_DRAW);


	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions + texcoords
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, noob::billboard_buffer::stride, reinterpret_cast<const void *>(0));
	// Colours
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, noob::billboard_buffer::stride, reinterpret_cast<const void *>(sizeof(noob::vec4f)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	check_error_gl();

	billboards.push_back(results);

	return noob::billboard_buffer_handle::make(billboards.size() - 1);
}


void noob::graphics::resize_buffers(noob::instanced_model_handle Handle, uint32_t NumInstances) noexcept(true)
{
	if (Handle.index() < instanced_models.size())
	{
		noob::instanced_model m = instanced_models[Handle.index()];

		m.n_instances = NumInstances;
		instanced_models[Handle.index()] = m;

		glBindVertexArray(m.vao);

		// Setup colours VBO:
		glBindBuffer(GL_ARRAY_BUFFER, m.colours_vbo);
		glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::instanced_model::colours_stride, nullptr, GL_DYNAMIC_DRAW);

		// Setup matrices VBO:
		glBindBuffer(GL_ARRAY_BUFFER, m.matrices_vbo);
		glBufferData(GL_ARRAY_BUFFER, NumInstances * noob::instanced_model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);

		check_error_gl();

		glBindVertexArray(0);
	}
}


void noob::graphics::resize_buffers(noob::billboard_buffer_handle Handle, uint32_t MaxBillboards) noexcept(true)
{
	if (Handle.index() < billboards.size())
	{
		noob::billboard_buffer bb = billboards[Handle.index()];
		if (bb.max < MaxBillboards)
		{
			bb.max = MaxBillboards;
			billboards[Handle.index()] = bb;

			glBindVertexArray(bb.vao);
			glBindBuffer(GL_ARRAY_BUFFER, bb.vertices_vbo);
			glBufferData(GL_ARRAY_BUFFER, bb.max * noob::billboard_buffer::stride, nullptr, GL_DYNAMIC_DRAW);
			check_error_gl();

			glBindVertexArray(0);
		}
	}
}


void noob::graphics::resize_terrain(uint32_t MaxVerts) noexcept(true)
{
	max_terrain_verts = MaxVerts;

	glBindVertexArray(terrain.vao);
	glBindBuffer(GL_ARRAY_BUFFER, terrain.vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_terrain_verts * noob::terrain_model::stride, nullptr, GL_DYNAMIC_DRAW);
	check_error_gl();

	glBindVertexArray(0);
}


uint32_t noob::graphics::get_max_terrain_verts() const noexcept(true)
{
	return max_terrain_verts;
}


void noob::graphics::set_terrain_uniforms(const noob::terrain_shading Shading) noexcept(true)
{
	terrain_unis = Shading;
}

void noob::graphics::upload_terrain_uniforms() const noexcept(true)
{
	const noob::mat4f mvp = proj_mat * view_mat;
	glUniform1i(u_texture_0, 0);
	glUniformMatrix4fv(u_mvp_terrain, 1, false, &mvp[0]);
	glUniform4fv(u_colour_0, 1, &terrain_unis.colours[0][0]);
	glUniform4fv(u_colour_1, 1, &terrain_unis.colours[1][0]);
	glUniform4fv(u_colour_2, 1, &terrain_unis.colours[2][0]);
	glUniform4fv(u_colour_3, 1, &terrain_unis.colours[3][0]);
	glUniform3fv(u_blend_0, 1, &terrain_unis.texture_weights[0]);
	glUniform2fv(u_blend_1, 1, &terrain_unis.colour_offsets[0]);
	glUniform3fv(u_tex_scales, 1, &terrain_unis.texture_scales[0]);
	glUniform3fv(u_eye_pos_terrain, 1, &eye_pos[0]);
	glUniform3fv(u_light_directional_terrain, 1, &light_direction[0]);
}


void noob::graphics::set_light_direction(const noob::vec3f& Arg) noexcept(true)
{
	light_direction = noob::normalize(Arg);
}


noob::gpu_write_buffer noob::graphics::map_matrices_buffer(noob::instanced_model_handle Handle, uint32_t Min, uint32_t Max) const noexcept(true)
{
	if (Handle.index() < instanced_models.size())
	{
		const noob::instanced_model m = instanced_models[Handle.index()];
		const uint32_t stride_in_bytes = noob::instanced_model::matrices_stride;
		glBindBuffer(GL_ARRAY_BUFFER, m.matrices_vbo);
		check_error_gl();

		const uint32_t total_size = stride_in_bytes * m.n_instances;
		float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
		check_error_gl();

		if (ptr != nullptr)
		{
			return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
		}
	}
	return noob::gpu_write_buffer::make_invalid();	
}


noob::gpu_write_buffer noob::graphics::map_colours_buffer(noob::instanced_model_handle Handle, uint32_t Min, uint32_t Max) const noexcept(true)
{
	if (Handle.index() < instanced_models.size())
	{
		const noob::instanced_model m = instanced_models[Handle.index()];
		const uint32_t stride_in_bytes = noob::instanced_model::colours_stride;
		glBindBuffer(GL_ARRAY_BUFFER, m.colours_vbo);
		check_error_gl();

		const uint32_t total_size = stride_in_bytes * m.n_instances;
		float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
		check_error_gl();

		if (ptr != nullptr)
		{
			return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
		}
	}

	return noob::gpu_write_buffer::make_invalid();	
}


noob::gpu_write_buffer noob::graphics::map_terrain_buffer(uint32_t Min, uint32_t Max) const noexcept(true)
{
	if (Min < Max && Max < max_terrain_verts)
	{
		// glBindVertexArray(terrain_model.vao);
		glBindBuffer(GL_ARRAY_BUFFER, terrain.vertices_vbo);
		check_error_gl();

		const uint32_t total_size = noob::terrain_model::stride * (Max - Min);
		float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
		check_error_gl();

		if (ptr != nullptr)
		{
			return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
		}
	}
	else
	{
		noob::logger::log(noob::importance::WARNING, "[Graphics] Tried to use invalid value for mapping terrain buffer.");
	}

	return noob::gpu_write_buffer::make_invalid();	
}


// TODO
noob::gpu_write_buffer noob::graphics::map_billboards(noob::billboard_buffer_handle Handle, uint32_t Min, uint32_t Max) const noexcept(true)
{
	if (Handle.index() < billboards.size())
	{
		const noob::billboard_buffer buf = billboards[Handle.index()];

		if (Min < Max && Max < buf.max)
		{
			glBindBuffer(GL_ARRAY_BUFFER, buf.vertices_vbo);
			check_error_gl();

			const uint32_t total_size = noob::billboard_buffer::stride * (Max - Min);
			float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, Min, Max, GL_MAP_WRITE_BIT));
			check_error_gl();

			if (ptr != nullptr)
			{
				return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
			}
		}
		else
		{
			noob::logger::log(noob::importance::WARNING, "[Graphics] Tried to use invalid value for mapping terrain buffer.");
		}

		return noob::gpu_write_buffer::make_invalid();	

	}

	return noob::gpu_write_buffer::make_invalid();	
}


void noob::graphics::unmap_buffer() const noexcept(true)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);

	check_error_gl();
}


noob::texture_2d_handle noob::graphics::texture_2d(const noob::texture_loader_2d& TextureLoader, bool GenMips, bool Immutable) noexcept(true)
{
	glBindVertexArray(0);

	const GLuint texture_id = prep_texture();

	glBindTexture(GL_TEXTURE_2D, texture_id);

	const noob::vec2ui dims = TextureLoader.dimensions();

	uint32_t mips = 1;
	if (TextureLoader.mips() || GenMips)
	{
		mips = noob::get_num_mips(dims);
	}

	if (Immutable)
	{
		glTexStorage2D(GL_TEXTURE_2D, mips, get_pixel_format_sized(TextureLoader.format()), dims[0], dims[1]);
		check_error_gl();	
	}

	if (TextureLoader.compressed())
	{
		uint32_t data_size;
		if (TextureLoader.format() == noob::pixel_format::RGBA8_COMPRESSED || TextureLoader.format() == noob::pixel_format::SRGBA8_COMPRESSED)
		{
			data_size = get_compressed_size_rgba8(dims[0], dims[1]);
		}
		else
		{
			data_size = get_compressed_size_rgb8(dims[0], dims[1]);
		}

		if (Immutable)
		{
			glCompressedTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dims[0], dims[1], get_pixel_format_sized(TextureLoader.format()), data_size, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));
			check_error_gl();		
		}
	}
	else
	{
		if (Immutable)
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, dims[0], dims[1], get_pixel_format_unsized(TextureLoader.format()), GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));
			check_error_gl();					
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, get_pixel_format_unsized(TextureLoader.format()), dims[0], dims[1], 0, get_pixel_format_unsized(TextureLoader.format()), GL_UNSIGNED_BYTE, reinterpret_cast<const GLvoid*>(TextureLoader.buffer()));
			check_error_gl();			
		}
	}

	if (GenMips)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
		check_error_gl();		
	}

	noob::texture_2d t(texture_id, noob::texture_info(mips > 1, TextureLoader.format()), dims[0], dims[1]);
	textures_2d.push_back(t);
	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created 2D texture of size ", noob::to_string(dims[0]), "-", noob::to_string(dims[1]), " with format ", noob::to_string(TextureLoader.format()), " and ", noob::to_string(mips) ," mips."));

	return noob::texture_2d_handle::make(textures_2d.size() - 1);
}


noob::texture_array_2d_handle noob::graphics::texture_array_2d(const noob::vec2ui Dims, uint32_t Indices, const noob::texture_info TexInfo) noexcept(true)
{
	const GLenum fmt = get_pixel_format_sized(TexInfo.pixels);

	glBindVertexArray(0);

	const GLuint texture_id = prep_texture();
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_id);

	uint32_t mips = 1;
	if (TexInfo.mips)
	{
		mips = noob::get_num_mips(Dims);
	}

	glTexStorage3D(GL_TEXTURE_2D_ARRAY, mips, fmt, Dims[0], Dims[1], Indices);

	check_error_gl();

	noob::texture_array_2d t(texture_id, TexInfo, Dims[0], Dims[1], Indices);

	texture_arrays_2d.push_back(t);

	return noob::texture_array_2d_handle::make(texture_arrays_2d.size() - 1);
}


noob::texture_3d_handle noob::graphics::texture_3d(const noob::vec3ui Dims, const noob::texture_info TexInfo) noexcept(true)
{
	const GLenum fmt = get_pixel_format_sized(TexInfo.pixels);

	glBindVertexArray(0);

	const GLuint texture_id = prep_texture();
	glBindTexture(GL_TEXTURE_3D, texture_id);

	uint32_t mips = 1;
	if (TexInfo.mips)
	{
		mips = noob::get_num_mips(Dims);
	}

	// TODO: Add check for devices that don't support compressed 3D texture storage. For now, we'll find out the exciting way.

	glTexStorage3D(GL_TEXTURE_3D, mips, fmt, Dims[0], Dims[1], Dims[2]);

	check_error_gl();

	noob::texture_3d t(texture_id, TexInfo, Dims[0], Dims[1], Dims[2]);

	textures_3d.push_back(t);

	return noob::texture_3d_handle::make(textures_3d.size() - 1);
}


void noob::graphics::bind_texture(noob::texture_2d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_2D, textures_2d[Handle.index()].driver_handle);
	check_error_gl();
}


void noob::graphics::bind_texture(noob::texture_array_2d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_2D_ARRAY, texture_arrays_2d[Handle.index()].driver_handle);
	check_error_gl();
}


void noob::graphics::bind_texture(noob::texture_3d_handle Handle) const noexcept(true)
{
	glBindTexture(GL_TEXTURE_3D, textures_3d[Handle.index()].driver_handle);
	check_error_gl();
}

/*
   void noob::graphics::texture_data(noob::texture_2d_handle Handle, uint32_t Mip, const noob::vec2ui Offsets, const noob::vec2ui Dims, const uint8_t* DataPtr) const noexcept(true)
   {
   if (Handle.index() < textures_2d.size())
   {
   }
   }
   */

void noob::graphics::texture_data(noob::texture_array_2d_handle Handle, uint32_t Mip, uint32_t Index, const noob::vec2ui Offset, const noob::vec2ui Dims, const uint8_t* DataPtr) const noexcept(true)
{
	// void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);

	// void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * data);

	check_error_gl();
}


void noob::graphics::texture_data(noob::texture_3d_handle Handle, uint32_t Mip, const noob::vec3ui Offsets, const noob::vec3ui Dims, const uint8_t* DataPtr) const noexcept(true)
{
	// void glCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid * data);

	// void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid * data);

	check_error_gl();
}

void noob::graphics::texture_base_level(uint32_t Mip) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, Mip);
	check_error_gl();
}


void noob::graphics::texture_compare_mode(noob::tex_compare_mode CompareMode) const noexcept(true)
{
	GLint param;
	switch (CompareMode)
	{
		case(noob::tex_compare_mode::COMPARE_REF_TO_TEXTURE):
			{
				param = GL_COMPARE_REF_TO_TEXTURE;
				break;
			}
		case(noob::tex_compare_mode::NONE):
			{
				param = GL_NONE;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, param);

	check_error_gl();
}


void noob::graphics::texture_compare_func(noob::tex_compare_func CompareFunc) const noexcept(true)
{
	GLenum param;
	switch(CompareFunc)
	{
		case(noob::tex_compare_func::LEQUAL):
			{
				param = GL_LEQUAL;
				break;
			}
		case(noob::tex_compare_func::GEQUAL):
			{
				param = GL_GEQUAL;
				break;
			}
		case(noob::tex_compare_func::LESS):
			{
				param = GL_LESS;
				break;
			}
		case(noob::tex_compare_func::GREATER):
			{
				param = GL_GREATER;
				break;
			}
		case(noob::tex_compare_func::EQUAL):
			{
				param = GL_EQUAL;
				break;
			}
		case(noob::tex_compare_func::NOTEQUAL):
			{
				param = GL_NOTEQUAL;
				break;
			}
		case(noob::tex_compare_func::ALWAYS):
			{
				param = GL_ALWAYS;
				break;
			}
		case(noob::tex_compare_func::NEVER):
			{
				param = GL_NEVER;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, param);

	check_error_gl();
}


void noob::graphics::texture_min_filter(noob::tex_min_filter MinFilter) const noexcept(true)
{
	GLenum param;
	switch(MinFilter)
	{
		case(noob::tex_min_filter::NEAREST):
			{
				param = GL_NEAREST;
				break;
			}
		case(noob::tex_min_filter::LINEAR):
			{
				param = GL_LINEAR;
				break;
			}
		case(noob::tex_min_filter::NEAREST_MIPMAP_NEAREST):
			{
				param = GL_NEAREST_MIPMAP_NEAREST;
				break;
			}
		case(noob::tex_min_filter::LINEAR_MIPMAP_NEAREST):
			{
				param = GL_LINEAR_MIPMAP_NEAREST;
				break;
			}
		case(noob::tex_min_filter::NEAREST_MIPMAP_LINEAR):
			{
				param = GL_NEAREST_MIPMAP_LINEAR;
				break;
			}
		case(noob::tex_min_filter::LINEAR_MIPMAP_LINEAR):
			{
				param = GL_LINEAR_MIPMAP_LINEAR;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param);

	check_error_gl();
}


void noob::graphics::texture_mag_filter(noob::tex_mag_filter MagFilter) const noexcept(true)
{
	GLenum param;
	switch(MagFilter)
	{
		case(noob::tex_mag_filter::NEAREST):
			{
				param = GL_NEAREST;
				break;
			}
		case(noob::tex_mag_filter::LINEAR):
			{
				param = GL_LINEAR;
				break;
			}
	};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param);

	check_error_gl();
}


void noob::graphics::texture_min_lod(int32_t MinLod) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, MinLod);
	check_error_gl();
}


void noob::graphics::texture_max_lod(int32_t MaxLod) const noexcept(true)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, MaxLod);
	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 2> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 3> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_SWIZZLE_B;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 4> Swizzles) const noexcept(true)
{
	for (uint32_t i = 0; i < 4; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_SWIZZLE_R;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_SWIZZLE_G;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_SWIZZLE_B;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (3):
				{
					pname = GL_TEXTURE_SWIZZLE_A;
					param = get_swizzle(Swizzles[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 2> WrapModes) const noexcept(true)
{
	for (uint32_t i = 0; i < 2; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_WRAP_S;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_WRAP_T;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_2D, pname, param);
					break;
				}
		};
	}

	check_error_gl();
}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 3> WrapModes) const noexcept(true)
{
	for (uint32_t i = 0; i < 3; ++i)
	{
		GLenum pname, param;	
		switch (i)
		{
			case (0):
				{
					pname = GL_TEXTURE_WRAP_S;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}
			case (1):
				{
					pname = GL_TEXTURE_WRAP_T;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}
			case (2):
				{
					pname = GL_TEXTURE_WRAP_R;
					param = get_wrapping(WrapModes[i]);
					glTexParameteri(GL_TEXTURE_3D, pname, param);
					break;
				}

		};
	}


	check_error_gl();
}



void noob::graphics::texture_pack_alignment(uint32_t Arg) const noexcept(true)
{
	if (texture_packing_valid(Arg))
	{
		glPixelStorei(GL_PACK_ALIGNMENT, Arg);
	}
}


void noob::graphics::texture_unpack_alignment(uint32_t Arg) const noexcept(true)
{
	if (texture_packing_valid(Arg))
	{
		glPixelStorei(GL_UNPACK_ALIGNMENT, Arg); 
	}
}


void noob::graphics::generate_mips(noob::texture_2d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_2D);
}


void noob::graphics::generate_mips(noob::texture_array_2d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}


void noob::graphics::generate_mips(noob::texture_3d_handle) const noexcept(true)
{
	glGenerateMipmap(GL_TEXTURE_3D);
}


noob::graphics::program_handle noob::graphics::get_instanced_shader() const noexcept(true)
{
	return instanced_shader;
}

