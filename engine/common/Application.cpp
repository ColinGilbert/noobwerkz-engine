#include "Application.hpp"

noob::application* noob::application::app_pointer = nullptr;


noob::application::application() 
{
	app_pointer = this;
	paused = input_has_started = false;
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	time = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	finger_positions = { noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f), noob::vec2(0.0f,0.0f) };
	prefix = std::unique_ptr<std::string>(new std::string("./"));
	script_engine = asCreateScriptEngine();
	assert(script_engine > 0);
	// TODO: Uncomment once noob::filesystem is fixed
	// noob::filesystem::init(*prefix);
	set_init_script("init.as");
	view_mat = noob::look_at(noob::vec3(0, 50.0, -100.0), noob::vec3(0.0, 0.0, 0.0), noob::vec3(0.0, 1.0, 0.0));
}	


noob::application::~application()
{
	script_engine->ShutDownAndRelease();
	app_pointer = nullptr;
}


noob::application& noob::application::get()
{
	logger::log("application::get()");
	assert(app_pointer && "application not created!");
	return *app_pointer;
}


void noob::application::set_init_script(const std::string& name)
{
	script_name = name;
}


void noob::application::eval(const std::string& s)
{

}


void angel_message_callback(const asSMessageInfo *msg, void *param)
{
	std::string message_type;
	switch (msg->type)
	{
		case (asMSGTYPE_WARNING):
			{
				message_type = "WARN";
				break;
			}

		case (asMSGTYPE_INFORMATION):
			{
				message_type = "INFO";
				break;
			}

		default:
			{
				message_type = "ERR";
			}
	}
	fmt::MemoryWriter ww;
	ww << "[AngelScript callback] " << message_type << ": " << msg->message << ", script section: " << msg->section << " row = " << msg->row << " col: " << msg->col;
	logger::log(ww.str());
}


void noob::application::init()
{
	logger::log("[Application] Begin init.");
	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	globals::init();
	stage.init();
	voxels.init(512, 512, 512);

	// Used by AngelScript to capture the result of the last registration.
	int r;
	// Example: r = script_engine->RegisterGlobalFunction("", asFUNCTION(func_name), asCALL_CDECL);
	script_engine->SetMessageCallback(asFUNCTION(angel_message_callback), 0, asCALL_CDECL);

	RegisterStdString(script_engine);

	RegisterVector<float>("float", script_engine);
	RegisterVector<double>("double", script_engine);
	RegisterVector<int>("int", script_engine);
	RegisterVector<unsigned int>("uint", script_engine);
	// Does not work! TODO: Report on forums.
	// RegisterVector<bool>("bool", script_engine);

	// Neither those two seem to work. They register just fine but there's literally no output to the commandline.
	r = script_engine->RegisterGlobalFunction("void logger(const string& in)", asFUNCTION(logger::log), asCALL_CDECL); assert (r >= 0);
	// r = script_engine->RegisterGlobalFunction("void log(const string& in)", asFUNCTIONPR(logger::log, (const std::string&), void), asCALL_CDECL); assert (r >= 0);

	// struct uniform
	// {
	// 	void init(const std::string& name, bgfx::UniformType::Enum _type, uint16_t _count = 1)
	// 	{
	// 		handle = bgfx::createUniform(name.c_str(), type, _count);
	// 		type = _type;
	// 		count = _count;
	// 	}

	// 	bgfx::UniformHandle handle;
	// 	bgfx::UniformType::Enum type;
	// 	uint16_t count;
	// };

	// struct sampler
	// {
	// 	void init(const std::string& name)
	// 	{
	// 		handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Int1);
	// 	}
	// 	bgfx::UniformHandle handle;
	// };

	r = script_engine->RegisterObjectType("texture", sizeof(noob::graphics::texture), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::graphics::texture>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	r = script_engine->RegisterObjectProperty("texture", "uint16 handle", asOFFSET(noob::graphics::texture, handle)); assert(r >= 0);

	// struct texture
	// {
	// 	bgfx::TextureHandle handle;
	// };

	// struct shader
	// {
	// 	bgfx::ProgramHandle program;
	// 	std::vector<noob::graphics::uniform> uniforms;
	// 	std::vector<noob::graphics::sampler> samplers;	
	// };

	// static void init(uint32_t width, uint32_t height);
	// static void destroy();
	// static void frame(uint32_t width, uint32_t height);

	// ---------------- Asset loaders (builds from files and returns handles) -----------------
	// static bgfx::ShaderHandle load_shader(const std::string& filename);
	// static bgfx::ProgramHandle load_program(const std::string& vs_filename, const std::string& fs_filename);

	// static noob::graphics::texture load_texture(const std::string& friendly_name, const std::string& filename, uint32_t flags);
	r = script_engine->RegisterGlobalFunction("texture load_texture(const string& in, const string& in, uint)", asFUNCTION(noob::graphics::load_texture), asCALL_CDECL); assert (r >= 0);

	// ---------------- Asset creators (make assets available from getters) ----------------
	// static bool add_sampler(const std::string&);
	// static bool add_uniform(const std::string& name, bgfx::UniformType::Enum type, uint16_t count);
	// static bool add_shader(const std::string&, const noob::graphics::shader&);

	// ---------------- Getters -----------------
	// static noob::graphics::shader get_shader(const std::string&);
	// static noob::graphics::texture get_texture(const std::string&);
	r = script_engine->RegisterGlobalFunction("texture get_texture(const string& in)", asFUNCTION(noob::graphics::get_texture), asCALL_CDECL); assert (r >= 0);
	
	// static noob::graphics::uniform get_uniform(const std::string&);
	// static noob::graphics::sampler get_sampler(const std::string&);

	// ---------------- Checkers ----------------
	// static bool is_valid(const noob::graphics::uniform&);
	// static bool is_valid(const noob::graphics::sampler&);

	r = script_engine->RegisterEnum("csg_op"); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "UNION", 0); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "DIFFERENCE", 1); assert(r >= 0 );
	r = script_engine->RegisterEnumValue("csg_op", "INTERSECTION", 2); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec2", sizeof(vec2), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec2>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec2>("vec2", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec2_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_CONSTRUCT, "void f(float, float)", asFUNCTION(as_vec2_constructor_wrapper_float_2), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec2", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec2_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec2", "void set_opIndex(int, float)", asMETHOD(noob::vec2, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec2", "float get_opIndex(int) const", asMETHOD(noob::vec2, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec3", sizeof(vec3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec3>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec3>("vec3", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec3_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(float, float, float)", asFUNCTION(as_vec3_constructor_wrapper_float_3), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(vec2, float)", asFUNCTION(as_vec3_constructor_wrapper_vec2_float), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	// r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_CONSTRUCT, "void f(vec4)", asFUNCTION(as_vec2_constructor_wrapper_float_2), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec3", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec3_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opAdd(float) const", asMETHODPR(noob::vec3, operator+, (float) const, noob::vec3), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opAdd(const vec3& in) const", asMETHODPR(noob::vec3, operator+, (const noob::vec3&) const, noob::vec3), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opAddAssign(const vec3& in)", asMETHOD(noob::vec3, operator+=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opSub(const vec3& in) const", asMETHODPR(noob::vec3, operator-, (float) const, noob::vec3), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opSub(float) const", asMETHODPR(noob::vec3, operator-, (const noob::vec3&) const, noob::vec3), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opSubAssign(const vec3& in)", asMETHOD(noob::vec3, operator-=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opMult(float)", asMETHOD(noob::vec3, operator*), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opMultAssign(float)", asMETHOD(noob::vec3, operator*=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3 opDiv(float)", asMETHOD(noob::vec3, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "vec3& opAssign(const vec3& in)", asMETHOD(noob::vec3, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("vec3", "void set_opIndex(int, float)", asMETHOD(noob::vec3, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec3", "float get_opIndex(int) const", asMETHOD(noob::vec3, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("vec4", sizeof(vec4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::vec4>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::vec4>("vec4", script_engine);
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_vec4_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(as_vec4_constructor_wrapper_float_4), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(vec2, float, float)", asFUNCTION(as_vec4_constructor_wrapper_vec2_float_2), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_CONSTRUCT, "void f(vec3, float)", asFUNCTION(as_vec4_constructor_wrapper_vec3_float), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("vec4", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_vec4_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "void set_opIndex(int, float)", asMETHOD(noob::vec4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "float get_opIndex(int) const", asMETHOD(noob::vec4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("mat3", sizeof(mat3), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::mat3>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::mat3>("mat3", script_engine);

	r = script_engine->RegisterObjectType("mat4", sizeof(mat4), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::mat4>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::mat4>("mat4", script_engine);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const vec4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::vec4&) const, noob::vec4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const mat4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::mat4&) const, noob::mat4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "void set_opIndex(int, float)", asMETHOD(noob::mat4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("mat4", "float get_opIndex(int) const", asMETHOD(noob::mat4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("versor", sizeof(versor), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::versor>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);//| asOBJ_POD | asGetTypeTraits<noob::versor>()); assert(r >= 0 );
	RegisterVector<noob::versor>("versor", script_engine);
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(as_versor_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_CONSTRUCT, "void f(float, float, float, float)", asFUNCTION(as_versor_constructor_wrapper_float_4), asCALL_CDECL_OBJLAST); assert( r >= 0);
	r = script_engine->RegisterObjectBehaviour("versor", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_versor_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("versor", "vec3 opDiv(float) const", asMETHOD(noob::versor, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(float) const", asMETHODPR(noob::versor, operator*, (float) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(const versor& in) const", asMETHODPR(noob::versor, operator*, (const noob::versor&) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor& opAssign(const versor& in)", asMETHOD(noob::versor, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "void set_opIndex(int, float)", asMETHOD(noob::versor, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("versor", "float get_opIndex(int) const", asMETHOD(noob::versor, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("bbox", sizeof(cubic_region), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::cubic_region>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::cubic_region>("bbox", script_engine);
	r = script_engine->RegisterObjectProperty("bbox", "vec3 min", asOFFSET(noob::bbox, min)); assert (r >= 0);
	r = script_engine->RegisterObjectProperty("bbox", "vec3 max", asOFFSET(noob::bbox, max)); assert (r >= 0);
	r = script_engine->RegisterObjectProperty("bbox", "vec3 center", asOFFSET(noob::bbox, center)); assert (r >= 0);

	r = script_engine->RegisterObjectType("cubic_region", sizeof(cubic_region), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::cubic_region>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::cubic_region>("cubic_region", script_engine);
	r = script_engine->RegisterObjectProperty("cubic_region", "vec3 lower_corner", asOFFSET(noob::cubic_region, lower_corner)); assert (r >= 0);
	r = script_engine->RegisterObjectProperty("cubic_region", "vec3 upper_corner", asOFFSET(noob::cubic_region, upper_corner)); assert (r >= 0);

	r = script_engine->RegisterGlobalFunction("bool compare_floats(float, float)", asFUNCTION(compare_floats), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float length(const vec3& in)", asFUNCTION(length_squared), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float length_squared(const vec3& in)", asFUNCTION(length), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 normalize(const vec3& in)", asFUNCTIONPR(normalize, (const vec3&), vec3), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float dot(const vec3& in, const vec3& in)", asFUNCTIONPR(dot, (const vec3&, const vec3&), float), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 cross(const vec3& in, const vec3& in)", asFUNCTION(cross), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float get_squared_dist(const vec3& in, const vec3& in)", asFUNCTION(get_squared_dist), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float direction_to_heading(const vec3& in)", asFUNCTION(direction_to_heading), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 heading_to_direction(float)", asFUNCTION(heading_to_direction), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("bool linearly_dependent(const vec3& in, const vec3& in, const vec3& in)", asFUNCTION(linearly_dependent), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat3 zero_mat3()", asFUNCTION(zero_mat3), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat3 identity_mat3()", asFUNCTION(identity_mat3), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 zero_mat4()", asFUNCTION(zero_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 identity_mat4()", asFUNCTION(identity_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("float determinant(const mat4& in)", asFUNCTION(determinant), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 inverse(const mat4& in)", asFUNCTION(inverse), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 transpose(const mat4& in)", asFUNCTION(transpose), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 translate(const mat4& in, const vec3& in)", asFUNCTION(translate), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_x_deg(const mat4& in, float)", asFUNCTION(rotate_x_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_y_deg(const mat4& in, float)", asFUNCTION(rotate_y_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 rotate_z_deg(const mat4& in, float)", asFUNCTION(rotate_z_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 scale(const mat4& in, const vec3& in)", asFUNCTION(scale), asCALL_CDECL); assert(r >= 0);
	// vec3 translation_from_mat4(const mat4& m);
	// vec3 scale_from_mat4(const mat4& m);
	r = script_engine->RegisterGlobalFunction("mat4 look_at(const vec3& in, const vec3& in, const vec3& in)", asFUNCTION(look_at), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 perspective(float, float, float, float)", asFUNCTION(perspective), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 scale(float, float, float, float, float, float)", asFUNCTION(ortho), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_axis_rad(float, float, float, float)", asFUNCTION(quat_from_axis_rad), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_axis_deg(float, float, float, float)", asFUNCTION(quat_from_axis_deg), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor quat_from_mat4(const mat4& in)", asFUNCTION(quat_from_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("mat4 quat_to_mat4(const versor& in)", asFUNCTION(quat_to_mat4), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("float dot(const versor& in, const versor& in)", asFUNCTIONPR(dot, (const versor&, const versor&), float), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor normalize(const versor& in)", asFUNCTIONPR(normalize, (const versor&), versor), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor slerp(const versor& in, const versor& in, float)", asFUNCTION(slerp), asCALL_CDECL); assert(r >= 0);

	r = script_engine->RegisterObjectType("basic_mesh", sizeof(basic_mesh), asOBJ_VALUE | asGetTypeTraits<noob::basic_mesh>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::basic_mesh>("basic_mesh", script_engine);
	r = script_engine->RegisterObjectBehaviour("basic_mesh", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_basic_mesh_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("basic_mesh", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_basic_mesh_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "basic_mesh& opAssign(const basic_mesh& in)", asMETHODPR(noob::basic_mesh, operator=, (const noob::basic_mesh&),noob::basic_mesh&), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_vertex(uint)", asMETHOD(noob::basic_mesh, get_vertex), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_normal(uint)", asMETHOD(noob::basic_mesh, get_normal), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "vec3 get_texcoord(uint)", asMETHOD(noob::basic_mesh, get_texcoord), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "uint get_index(uint)", asMETHOD(noob::basic_mesh, get_index), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_vertex(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_vertex), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_normal(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_normal), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_texcoord(uint, const vec3& in)", asMETHOD(noob::basic_mesh, set_texcoord), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void set_index(uint, uint)", asMETHOD(noob::basic_mesh, set_index), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "double get_volume()", asMETHOD(noob::basic_mesh, get_volume), asCALL_THISCALL); assert( r >= 0 );
	// void decimate(const std::string& filename, size_t num_verts) const;
	// noob::basic_mesh decimate(size_t num_verts) const;
	r = script_engine->RegisterObjectMethod("basic_mesh", "string save()", asMETHODPR(noob::basic_mesh, save, (void) const, std::string), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void save(const string& in)", asMETHODPR(noob::basic_mesh, save, (const std::string&) const, void), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "bool load_mem(const string& in, const string& in)", asMETHODPR(noob::basic_mesh, load_mem, (const std::string&, const std::string&), bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void transform(const mat4& in)", asMETHOD(noob::basic_mesh, transform), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void normalize()", asMETHOD(noob::basic_mesh, normalize), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void to_origin()", asMETHOD(noob::basic_mesh, to_origin), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void translate(const vec3& in)", asMETHOD(noob::basic_mesh, translate), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void rotate(const versor& in)", asMETHOD(noob::basic_mesh, rotate), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void scale(const vec3& in)", asMETHOD(noob::basic_mesh, scale), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "bbox get_bbox() const", asMETHOD(noob::basic_mesh, get_bbox), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectType("face_handle", sizeof(noob::active_mesh::face_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::face_handle>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::face_handle>("face_handle", script_engine);

	r = script_engine->RegisterObjectType("vertex_handle", sizeof(noob::active_mesh::vertex_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::vertex_handle>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::vertex_handle>("vertex_handle", script_engine);

	r = script_engine->RegisterObjectType("edge_handle", sizeof(noob::active_mesh::edge_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::edge_handle>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::edge_handle>("edge_handle", script_engine);

	r = script_engine->RegisterObjectType("halfedge_handle", sizeof(noob::active_mesh::halfedge_handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::active_mesh::halfedge_handle>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh::halfedge_handle>("halfedge_handle", script_engine);


	r = script_engine->RegisterObjectType("active_mesh", sizeof(active_mesh), asOBJ_VALUE | asGetTypeTraits<noob::active_mesh>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	RegisterVector<noob::active_mesh>("active_mesh", script_engine);
	r = script_engine->RegisterObjectBehaviour("active_mesh", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_active_mesh_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("active_mesh", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_active_mesh_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "active_mesh& opAssign(const active_mesh& in)", asMETHODPR(noob::active_mesh, operator=, (const noob::active_mesh&),noob::active_mesh&), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void reset()", asMETHOD(noob::active_mesh, reset), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void from_basic_mesh(const basic_mesh& in)", asMETHOD(noob::active_mesh, from_basic_mesh), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vertex_handle add_vertex(const vec3& in)", asMETHOD(noob::active_mesh, add_vertex), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void add_face(const vector_vec3& in)", asMETHODPR(noob::active_mesh, add_face, (const std::vector<noob::vec3>&), noob::active_mesh::face_handle), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "void add_face(const vector_vertex_handle& in)", asMETHODPR(noob::active_mesh, add_face, (const std::vector<noob::active_mesh::vertex_handle>&), noob::active_mesh::face_handle), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool vertex_exists(const vec3& in) const", asMETHODPR(noob::active_mesh, vertex_exists, (const noob::vec3&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool vertex_exists(vertex_handle) const", asMETHODPR(noob::active_mesh, vertex_exists, (const noob::active_mesh::vertex_handle) const, bool), asCALL_THISCALL); assert( r >= 0 );
	// std::tuple<bool, noob::vec3> get_vertex(PolyMesh::VertexHandle) const;
	r = script_engine->RegisterObjectMethod("active_mesh", "vec3 get_face_normal(face_handle) const", asMETHOD(noob::active_mesh, get_face_normal), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vertex_handle get_vertex_handle(const vec3& in) const", asMETHOD(noob::active_mesh, get_vertex_handle), asCALL_THISCALL); assert(r >= 0);
	// Caution: This function has the potential to do lots of iterations, as it tests against all faces sharing the first vertex/
	r = script_engine->RegisterObjectMethod("active_mesh", "bool face_exists(const vector_vec3& in) const", asMETHODPR(noob::active_mesh, face_exists, (const std::vector<noob::vec3>&) const, bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("active_mesh", "bool face_exists(face_handle) const", asMETHODPR(noob::active_mesh, face_exists, (const noob::active_mesh::face_handle) const, bool), asCALL_THISCALL); assert( r >= 0 );
	// const std::tuple<bool, std::vector<noob::vec3>> get_face(PolyMesh::VertexHandle) const; 
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_vertices() const", asMETHOD(noob::active_mesh, num_vertices), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_half_edges() const", asMETHOD(noob::active_mesh, num_half_edges), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_edges() const", asMETHOD(noob::active_mesh, num_edges), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "uint64 num_faces() const", asMETHOD(noob::active_mesh, num_faces), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_vertex_handle get_vertex_handles_for_face(face_handle) const", asMETHOD(noob::active_mesh, get_vertex_handles_for_face), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_vec3 get_verts_for_face(face_handle) const", asMETHOD(noob::active_mesh, get_verts_for_face), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_edge_handle get_adjacent_edges(face_handle, face_handle) const", asMETHOD(noob::active_mesh, get_adjacent_edges), asCALL_THISCALL); assert(r >= 0);	
	r = script_engine->RegisterObjectMethod("active_mesh", "basic_mesh to_basic_mesh() const", asMETHOD(noob::active_mesh, to_basic_mesh), asCALL_THISCALL); assert(r >= 0);
	// std::vector<PolyMesh::FaceHandle> get_adjacent_faces(PolyMesh::FaceHandle) const;
	r = script_engine->RegisterObjectMethod("active_mesh", "vector_face_handle get_adjacent_faces(face_handle) const", asMETHOD(noob::active_mesh, get_adjacent_faces), asCALL_THISCALL); assert(r >= 0);
	// Destructive utiiities.
	r = script_engine->RegisterObjectMethod("active_mesh", "void make_hole(face_handle)", asMETHOD(noob::active_mesh, make_hole), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void fill_holes()", asMETHOD(noob::active_mesh, fill_holes), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void extrude(face_handle, float)", asMETHOD(noob::active_mesh, extrude), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void move_vertex(vertex_handle, const vec3& in)", asMETHOD(noob::active_mesh, move_vertex), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void move_vertices(const vector_vertex_handle& in, const vec3& in)", asMETHOD(noob::active_mesh, move_vertices), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("active_mesh", "void merge_adjacent_coplanars()", asMETHOD(noob::active_mesh, merge_adjacent_coplanars), asCALL_THISCALL); assert(r >= 0);

	// Mesh-related globals
	r = script_engine->RegisterGlobalFunction("basic_mesh sphere_mesh(float)", asFUNCTION(mesh_utils::sphere), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh box_mesh(float, float, float)", asFUNCTION(mesh_utils::box), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh cone_mesh(float, float)", asFUNCTION(mesh_utils::cone), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh cylinder_mesh(float, float)", asFUNCTION(mesh_utils::cylinder), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("basic_mesh hull_mesh(const vector_vec3& in)", asFUNCTION(mesh_utils::hull), asCALL_CDECL); assert(r >= 0);



	// TODO: Implement
	r = script_engine->RegisterGlobalFunction("basic_mesh csg(const basic_mesh& in, const basic_mesh& in, csg_op)", asFUNCTION(mesh_utils::csg), asCALL_CDECL); assert(r >= 0);

	r = script_engine->RegisterObjectType("body", sizeof(noob::body), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body>() | asOBJ_APP_CLASS_ALLINTS ); assert(r >= 0 );
	r = script_engine->RegisterEnum("body_type"); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "DYNAMIC", 0); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "STATIC", 1); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "KINEMATIC", 2); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "GHOST", 3); assert(r >= 0);

	r = script_engine->RegisterObjectType("body_info", sizeof(noob::body::info), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::body::info>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float mass", asOFFSET(noob::body::info, mass)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float friction", asOFFSET(noob::body::info, friction)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "float restitution", asOFFSET(noob::body::info, restitution)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 linear_factor", asOFFSET(noob::body::info, linear_factor)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 angular_factor", asOFFSET(noob::body::info, angular_factor)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 position", asOFFSET(noob::body::info, position)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 linear_velocity", asOFFSET(noob::body::info, linear_velocity)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "vec3 angular_velocity", asOFFSET(noob::body::info, angular_velocity)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "versor orientation", asOFFSET(noob::body::info, orientation)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "bool ccd", asOFFSET(noob::body::info, ccd)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("body_info", "body_type type", asOFFSET(noob::body::info, type)); assert( r >= 0 );

	r = script_engine->RegisterObjectType("light", sizeof(noob::light), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::light>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("light", "vec4 position", asOFFSET(noob::light, position)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("light", "vec4 specular", asOFFSET(noob::light, specular)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("light", "vec4 diffuse", asOFFSET(noob::light, diffuse)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("light", "vec4 ambient", asOFFSET(noob::light, ambient)); assert( r >= 0 );

	r = script_engine->RegisterObjectType("reflection", sizeof(noob::reflection), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflection>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("reflection", "vec4 specular", asOFFSET(noob::reflection, specular)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("reflection", "vec4 diffuse", asOFFSET(noob::reflection, diffuse)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("reflection", "vec4 ambient", asOFFSET(noob::reflection, ambient)); assert( r >= 0 );
	r = script_engine->RegisterObjectProperty("reflection", "float exponent", asOFFSET(noob::reflection, exponent)); assert( r >= 0 );

	r = script_engine->RegisterObjectType("basic_uniform", sizeof(noob::basic_renderer::uniform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::basic_renderer::uniform>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectProperty("basic_uniform", "vec4 colour", asOFFSET(noob::basic_renderer::uniform, colour)); assert(r >= 0);

	r = script_engine->RegisterObjectType("triplanar_gradmap_uniform", sizeof(noob::triplanar_gradient_map_renderer::uniform), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::triplanar_gradient_map_renderer::uniform>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 blend", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, blend)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 scales", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, scales)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "vec4 colour_positions", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, colour_positions)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("triplanar_gradmap_uniform", "texture texture_map", asOFFSET(noob::triplanar_gradient_map_renderer::uniform, texture_map)); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("triplanar_gradmap_uniform", "void set_colour(uint, const vec4& in)", asMETHOD(noob::triplanar_gradient_map_renderer::uniform, set_colour), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("triplanar_gradmap_uniform", "vec4 get_colour(uint) const", asMETHOD(noob::triplanar_gradient_map_renderer::uniform, get_colour), asCALL_THISCALL); assert( r >= 0 );	

	r = script_engine->RegisterObjectType("body_handle", sizeof(noob::bodies_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::bodies_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("mesh_handle", sizeof(noob::meshes_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::meshes_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("model_handle", sizeof(noob::basic_models_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::basic_models_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("animated_model_handle", sizeof(noob::animated_models_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::animated_models_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("skeletal_anim_handle", sizeof(noob::skeletal_anims_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::skeletal_anims_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("light_handle", sizeof(noob::lights_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::lights_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("reflection_handle", sizeof(noob::reflections_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::reflections_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);
	r = script_engine->RegisterObjectType("shader_handle", sizeof(noob::shaders_holder::handle), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::shaders_holder::handle>() | asOBJ_APP_CLASS_ALLINTS); assert ( r >= 0);

	r = script_engine->RegisterObjectType("stage", sizeof(noob::stage), asOBJ_VALUE); assert(r >= 0 );
	r = script_engine->RegisterObjectBehaviour("stage", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_stage_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("stage", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_stage_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void init()", asMETHOD(noob::stage, init), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void tear_down()", asMETHOD(noob::stage, tear_down), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void update(double)", asMETHOD(noob::stage, update), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void draw(float, float)", asMETHOD(noob::stage, draw), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint body(body_type, uint, float, const vec3& in, const versor& in, bool)", asMETHOD(noob::stage, _body), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterGlobalFunction("uint sphere(float)", asFUNCTION(globals::_sphere), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint box(float, float, float)", asFUNCTION(globals::_box), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint cylinder(float, float)", asFUNCTION(globals::_cylinder), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint cone(float, float)", asFUNCTION(globals::_cone), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint hull(const vector_vec3& in)", asFUNCTION(globals::_hull), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint static_trimesh(uint)", asFUNCTION(globals::_static_trimesh), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint add_mesh(const basic_mesh& in)", asFUNCTION(globals::_add_mesh), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint basic_model(uint)", asFUNCTION(globals::_basic_model), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("uint animated_model(const string& in)", asFUNCTION(globals::_animated_model), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint skeleton(const string& in)", asFUNCTION(globals::_skeleton), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_light(const light& in, const string& in)", asFUNCTION(globals::set_light), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("light get_light(const string& in)", asFUNCTION(globals::get_light), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_reflection(const reflection& in, const string& in)", asFUNCTION(globals::set_reflection), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("reflection get_reflection(const string& in)", asFUNCTION(globals::get_reflection), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_shader(const basic_uniform& in, const string& in)", asFUNCTIONPR(globals::set_shader, (const noob::basic_renderer::uniform&, const std::string&), void), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_shader(const triplanar_gradmap_uniform& in, const string& in)", asFUNCTIONPR(globals::set_shader, (const noob::triplanar_gradient_map_renderer::uniform&, const std::string&), void), asCALL_CDECL); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("uint get_shader(const string& in)", asFUNCTION(globals::_get_shader), asCALL_CDECL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("stage", "uint actor(uint, uint, const string& in)", asMETHOD(noob::stage, _actor), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint prop(uint, const string& in)", asMETHODPR(noob::stage, _prop, (unsigned int, const std::string&), unsigned int), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint prop(uint, uint, const string& in)", asMETHODPR(noob::stage, _prop, (unsigned int, unsigned int, const std::string&), unsigned int), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint scenery_from_mesh(uint, const vec3& in, const string& in, const versor& in)", asMETHOD(noob::stage, _scenery_from_mesh), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "uint scenery_from_shape(uint, const vec3& in, const string& in, const versor& in)", asMETHOD(noob::stage, _scenery_from_shape), asCALL_THISCALL); assert(r >= 0);

	r = script_engine->RegisterObjectProperty("stage", "bool show_origin", asOFFSET(noob::stage, show_origin)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("stage", "mat4 view_mat", asOFFSET(noob::stage, view_mat)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("stage", "mat4 projection_mat", asOFFSET(noob::stage, projection_mat)); assert(r >= 0);

	r = script_engine->RegisterGlobalProperty("stage default_stage", &stage); assert (r >= 0);

	r = script_engine->RegisterObjectType("voxel_world", sizeof(noob::voxel_world), asOBJ_VALUE); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_voxel_world_constructor_wrapper_basic), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	// r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_CONSTRUCT,  "void f(uint, uint, uint)", asFUNCTION(as_voxel_world_constructor_wrapper_uint_3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("voxel_world", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_voxel_world_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void init(uint, uint, uint)", asMETHOD(noob::voxel_world, init), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void set(uint, uint, uint, uint8)", asMETHOD(noob::voxel_world, set), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("voxel_world", "apply_to_region(uint, uint, uint, uint, uint, uint, std::function<uint8(uint, uint, uint)>)", asMETHOD(noob::voxel_world, apply_to_region), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void sphere(uint, uint, uint, uint, bool)", asMETHOD(noob::voxel_world, sphere), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void box(uint, uint, uint, uint, uint, uint, uint8)", asMETHOD(noob::voxel_world, box), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void get(uint, uint, uint)", asMETHOD(noob::voxel_world, safe_get), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "void get_unsafe(uint, uint, uint)", asMETHOD(noob::voxel_world, get), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_smooth()", asMETHOD(noob::voxel_world, extract_smooth), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_cubic()", asMETHODPR(noob::voxel_world, extract_cubic, (void) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_cubic(uint8)", asMETHODPR(noob::voxel_world, extract_cubic, (uint8_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_smooth(uint, uint, uint, uint, uint, uint)", asMETHOD(noob::voxel_world, extract_region_smooth), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_cubic(uint, uint, uint, uint, uint, uint)", asMETHODPR(noob::voxel_world, extract_region_cubic, (size_t, size_t, size_t, size_t, size_t, size_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("voxel_world", "basic_mesh extract_region_cubic(uint, uint, uint, uint, uint, uint, uint8)", asMETHODPR(noob::voxel_world, extract_region_cubic, (size_t, size_t, size_t, size_t, size_t, size_t, uint8_t) const, noob::basic_mesh), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterGlobalProperty("voxel_world voxels", &voxels); assert (r >= 0);



	logger::log("[Application] Done basic init.");
	bool b = user_init();
	// b = load_init_script();
}


void noob::application::update(double delta)
{
	gui.window_dims(window_width, window_height);
	stage.update(delta);
	user_update(delta);

	static double time_elapsed = 0.0;
	time_elapsed += delta;


	if (time_elapsed > 0.25)
	{
		boost::filesystem::path p;

		p += *prefix;
		p += script_name;

		boost::system::error_code ec;

		static std::time_t last_write = 0;
		std::time_t t = boost::filesystem::last_write_time(p, ec);
		if (ec != 0)
		{
			logger::log(fmt::format("[Application] - update() - error reading {0}: {1}", p.generic_string(), ec.message()));
		}	
		else if (last_write != t)
		{
		try
		{
			// script_module; = script_engine->GetModule("user_module", asGM_ALWAYS_CREATE);
			stage.tear_down();
			load_init_script();
		}
		catch(std::exception e)
		{
			logger::log(fmt::format("[Application]. Caught AngelScript exception: ", e.what()));
		}
			last_write = t;
		}
		time_elapsed = 0.0;
	}
}

bool noob::application::load_init_script()
{
	std::string user_message = "[Application] reloading script. Success? {0}";

	script_module = script_engine->GetModule(0, asGM_ALWAYS_CREATE);

	int r = script_module->AddScriptSection(script_name.c_str(), noob::utils::load_file_as_string(script_name).c_str());
	if (r < 0)
	{
		logger::log(fmt::format(user_message, "false - init script add section failed!"));
		return false;
	}

	r = script_module->Build();
	if (r < 0 )
	{
		logger::log(fmt::format(user_message, "false - init script compile failed!"));
		return false;
		// The build failed. The message stream will have received  
		// compiler errors that shows what needs to be fixed
	}
	asIScriptContext* ctx = script_engine->CreateContext();
	if (ctx == 0)
	{
		logger::log(fmt::format("false - failed to create context"));
	}

	asIScriptFunction* func = script_engine->GetModule(0)->GetFunctionByDecl("void main()");

	if (func == 0)
	{
		logger::log(fmt::format(user_message, "false - main() not found!"));
		return false;
	}

	r = ctx->Prepare(func);

	if( r < 0 ) 
	{
		logger::log(fmt::format(user_message, "false - failed to prepare the context!"));
		return false;
	}

	r = ctx->Execute();

	if( r != asEXECUTION_FINISHED )
	{
		// The execution didn't finish as we had planned. Determine why.
		if( r == asEXECUTION_ABORTED )
		{
			logger::log(fmt::format(user_message, "false - script aborted"));
		}

		else if( r == asEXECUTION_EXCEPTION )
		{
			fmt::MemoryWriter ww;
			ww << "false - script ended with an exception: ";

			asIScriptFunction* exception_func = ctx->GetExceptionFunction();

			ww << "function: " << exception_func->GetDeclaration() << ", ";
			ww << "module: " << exception_func->GetModuleName() << ", ";
			ww << "section: " << exception_func->GetScriptSectionName() << ", ";
			ww << "line: " << ctx->GetExceptionLineNumber() << ", ";
			ww << "description: " << ctx->GetExceptionString();

			logger::log(fmt::format(user_message, ww.str()));
		}

		else
		{
			fmt ::MemoryWriter ww;
			ww << "false - the script ended with reason code " << r;
			logger::log(fmt::format(user_message, ww.str()));
		}

		return false;
	}


	logger::log(fmt::format(user_message, "true"));
	return true;
}


void noob::application::draw()
{
	stage.projection_mat = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 10.0, 3000.0);
	stage.draw(window_width, window_height);
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		//logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		float damping = 360.0;
		noob::vec3 rotation(info.rotation);
		noob::vec3 translation(info.translation);
		stage.view_mat = noob::rotate_x_deg(stage.view_mat, -rotation[0]/damping);
		stage.view_mat = noob::rotate_y_deg(stage.view_mat, -rotation[1]/damping);
		stage.view_mat = noob::rotate_z_deg(stage.view_mat, -rotation[2]/damping);
		stage.view_mat = noob::translate(stage.view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
	}
}

// TODO: Refactor
void noob::application::step()
{
	timespec timeNow;
	clock_gettime(CLOCK_MONOTONIC, &timeNow);
	uint64_t uNowNano = timeNow.tv_sec * 1000000000ull + timeNow.tv_nsec;
	double delta = (uNowNano - time) * 0.000000001f;

	time = uNowNano;

	if (!paused)
	{
		update(delta);
	}

	draw();
}


void noob::application::pause()
{
	paused = true;
}


void noob::application::resume()
{
	paused = false;
}


void noob::application::set_archive_dir(const std::string& filepath)
{

	logger::log(fmt::format("[Application] Setting archive directory (\"{0}\")", filepath));
	prefix = std::unique_ptr<std::string>(new std::string(filepath));
	logger::log(fmt::format("[Application] Archive dir = {0}", *prefix));
}


void noob::application::touch(int pointerID, float x, float y, int action)
{
	if (input_has_started == true)
	{
		fmt::MemoryWriter w;
		w << "[Application] Touch - pointer ID = " << pointerID << ", " << x << ", " << y << ", action = " << action;
		logger::log(w.str());

		if (pointerID < 3)
		{
			finger_positions[pointerID] = noob::vec2(x,y);
		}
	}
	else input_has_started = true;
}


void noob::application::window_resize(uint32_t w, uint32_t h)
{
	window_width = w;
	window_height = h;
	if (window_height == 0) 
	{
		window_height = 1;
	}

	fmt::MemoryWriter ww;
	ww << "[Application] Resize window to (" << window_width << ", " << window_height << ")";
	logger::log(ww.str());
}


void noob::application::key_input(char c)
{

}
