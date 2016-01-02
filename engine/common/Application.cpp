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
	// assert(script_engine == 0); // TODO: Look into
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


void noob::application::eval(const std::string& string_to_eval)
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
	ww << "[AngelScript callback] " /* << msg->section << " row = " << msg->row << " col: " << msg->col << ", type: " */ << message_type << ": " << msg->message;
	logger::log(ww.str());
}


void noob::application::init()
{
	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	stage.init();
	voxels.init();

	// Used by ANgelScript to capture the result of the last registering.
	int r;
	//r = script_engine->RegisterGlobalFunction("", asFUNCTION(func_name), asCALL_CDECL);
	script_engine->SetMessageCallback(asFUNCTION(angel_message_callback), 0, asCALL_CDECL);

	RegisterStdString(script_engine);

	RegisterVector<float>("float", script_engine);
	RegisterVector<double>("double", script_engine);
	RegisterVector<int>("int", script_engine);
	RegisterVector<unsigned int>("uint", script_engine);
	// DOes not work!
	// RegisterVector<bool>("bool", script_engine);

	r = script_engine->RegisterObjectType("vec2", sizeof(vec2), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::vec2>("vec2", script_engine);
	// TODO: Constructors, operators, for vec2
	r = script_engine->RegisterObjectMethod("vec2", "void set_opIndex(int, float)", asMETHOD(noob::vec2, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec2", "float get_opIndex(int) const", asMETHOD(noob::vec2, get_opIndex), asCALL_THISCALL); assert(r >= 0 );
	
	
	r = script_engine->RegisterObjectType("vec3", sizeof(vec3), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::vec3>("vec3", script_engine);
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

	r = script_engine->RegisterObjectType("vec4", sizeof(vec4), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::vec4>("vec4", script_engine);

	r = script_engine->RegisterObjectMethod("vec4", "void set_opIndex(int, float)", asMETHOD(noob::vec4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("vec4", "float get_opIndex(int) const", asMETHOD(noob::vec4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("mat3", sizeof(mat3), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::mat3>("mat3", script_engine);

	r = script_engine->RegisterObjectType("mat4", sizeof(mat4), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::mat4>("mat4", script_engine);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const vec4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::vec4&) const, noob::vec4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "mat4 opMult(const mat4& in) const", asMETHODPR(noob::mat4, operator*, (const noob::mat4&) const, noob::mat4), asCALL_THISCALL); assert (r >= 0);
	r = script_engine->RegisterObjectMethod("mat4", "void set_opIndex(int, float)", asMETHOD(noob::mat4, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("mat4", "float get_opIndex(int) const", asMETHOD(noob::mat4, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("versor", sizeof(versor), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	RegisterVector<noob::versor>("versor", script_engine);
	r = script_engine->RegisterObjectMethod("versor", "vec3 opDiv(float) const", asMETHOD(noob::versor, operator/), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(float) const", asMETHODPR(noob::versor, operator*, (float) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor opMult(const versor& in) const", asMETHODPR(noob::versor, operator*, (const noob::versor&) const, noob::versor), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "versor& opAssign(const versor& in)", asMETHOD(noob::versor, operator=), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("versor", "void set_opIndex(int, float)", asMETHOD(noob::versor, set_opIndex), asCALL_THISCALL); assert(r >= 0 );
	r = script_engine->RegisterObjectMethod("versor", "float get_opIndex(int) const", asMETHOD(noob::versor, get_opIndex), asCALL_THISCALL); assert(r >= 0 );

	r = script_engine->RegisterObjectType("basic_mesh", sizeof(basic_mesh), asOBJ_VALUE); assert(r >= 0 );
	RegisterVector<noob::basic_mesh>("basic_mesh", script_engine);
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
	r = script_engine->RegisterObjectMethod("basic_mesh", "bool load_file(const string& in, const string& in)", asMETHODPR(noob::basic_mesh, load_file, (const std::string&, const std::string&), bool), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void transform(const vec3& in)", asMETHOD(noob::basic_mesh, transform), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void normalize()", asMETHOD(noob::basic_mesh, normalize), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void to_origin()", asMETHOD(noob::basic_mesh, to_origin), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void translate(const mat4& in)", asMETHOD(noob::basic_mesh, transform), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void rotate(const versor& in)", asMETHOD(noob::basic_mesh, rotate), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("basic_mesh", "void scale(const vec3& in)", asMETHOD(noob::basic_mesh, scale), asCALL_THISCALL); assert( r >= 0 );
	// noob::basic_mesh::bbox get_bbox() const { return bbox_info; }
	
	script_engine->RegisterGlobalFunction("basic_mesh sphere_mesh(float)", asFUNCTION(mesh_utils::sphere), asCALL_CDECL); assert(r >= 0);
	script_engine->RegisterGlobalFunction("basic_mesh cone_mesh(float, float)", asFUNCTION(mesh_utils::cone), asCALL_CDECL); assert(r >= 0);
	script_engine->RegisterGlobalFunction("basic_mesh box_mesh(float, float)", asFUNCTION(mesh_utils::cylinder), asCALL_CDECL); assert(r >= 0);
	script_engine->RegisterGlobalFunction("basic_mesh hull_mesh(const vector_vec3& in)", asFUNCTION(mesh_utils::hull), asCALL_CDECL); assert(r >= 0);
	
	r = script_engine->RegisterGlobalFunction("float length(const vec3& in)", asFUNCTION(length_squared), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float length_squared(const vec3& in)", asFUNCTION(length), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 normalize(const vec3& in)", asFUNCTIONPR(normalize, (const vec3&), vec3), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float dot(const vec3& in, const vec3& in)", asFUNCTIONPR(dot, (const vec3&, const vec3&), float), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 cross(const vec3& in, const vec3& in)", asFUNCTION(cross), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float get_squared_dist(const vec3& in, const vec3& in)", asFUNCTION(get_squared_dist), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("float direction_to_heading(const vec3& in)", asFUNCTION(direction_to_heading), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("vec3 heading_to_direction(float)", asFUNCTION(heading_to_direction), asCALL_CDECL); assert(r >= 0 );
	r = script_engine->RegisterGlobalFunction("bool linearly_dependent(const vec3& in, const vec3& in, const vec3& in)", asFUNCTION(linearly_dependent), asCALL_CDECL); assert(r >= 0);

	// matrix functions
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

	r = script_engine->RegisterObjectType("body_handle", sizeof(noob::bodies_holder::handle), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	r = script_engine->RegisterEnum("body_type"); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "DYNAMIC", 0); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "STATIC", 1); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "KINEMATIC", 2); assert(r >= 0);
	r = script_engine->RegisterEnumValue("body_type", "GHOST", 3); assert(r >= 0);

	RegisterVector<noob::bodies_holder::handle>("body_handle", script_engine);
	
	r = script_engine->RegisterObjectType("light", sizeof(noob::light), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
	r = script_engine->RegisterObjectType("reflection", sizeof(noob::reflection), asOBJ_VALUE | asOBJ_POD); assert (r >= 0);
	r = script_engine->RegisterObjectType("shader", sizeof(noob::prepared_shaders::info), asOBJ_VALUE | asOBJ_POD); assert ( r >= 0);
	r = script_engine->RegisterObjectType("stage", sizeof(noob::stage), asOBJ_VALUE); assert(r >= 0 );

	r = script_engine->RegisterObjectMethod("stage", "void init()", asMETHOD(noob::stage, init), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void tear_down()", asMETHOD(noob::stage, tear_down), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void update(double)", asMETHOD(noob::stage, update), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void draw(float, float)", asMETHOD(noob::stage, draw), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint body(body_type, uint, float, const vec3& in, const versor& in, bool)", asMETHOD(noob::stage, _body), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint sphere(float)", asMETHOD(noob::stage, _sphere), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint box(float, float, float)", asMETHOD(noob::stage, _box), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint cylinder(float, float)", asMETHOD(noob::stage, _cylinder), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint cone(float, float)", asMETHOD(noob::stage, _cone), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint hull(const vector_vec3& in)", asMETHOD(noob::stage, _hull), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint static_trimesh(uint)", asMETHOD(noob::stage, _static_trimesh), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint add_mesh(const basic_mesh& in)", asMETHOD(noob::stage, _add_mesh), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("stage", "uint basic_model(uint)", asMETHOD(noob::stage, _basic_model), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "uint animated_model(const string& in)", asMETHOD(noob::stage, _animated_model), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint skeleton(const string& in)", asMETHOD(noob::stage, _skeleton), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void set_light(const light& in, const string& in)", asMETHOD(noob::stage, set_light), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "light get_light(const string& in)", asMETHOD(noob::stage, get_light), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void set_reflection(const reflection& in, const string& in)", asMETHOD(noob::stage, set_reflection), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "reflection get_reflection(const string& in)", asMETHOD(noob::stage, get_reflection), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void set_shader(const shader& in, const string& in)", asMETHOD(noob::stage, set_shader), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint get_shader(const string& in)", asMETHOD(noob::stage, _get_shader), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint actor(uint, uint, const string& in)", asMETHOD(noob::stage, _actor), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint prop(uint, const string& in)", asMETHODPR(noob::stage, _prop, (unsigned int, const std::string&), unsigned int), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint prop(uint, uint, const string& in)", asMETHODPR(noob::stage, _prop, (unsigned int, unsigned int, const std::string&), unsigned int), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "uint scenery(uint, const vec3& in, const string& in, const versor& in)", asMETHOD(noob::stage, _scenery), asCALL_THISCALL); assert(r >= 0);
	// std::tuple<noob::basic_models_holder::handle,noob::vec3> stage.get_model(const noob::shapes_holder::handle);
	// bool stage.show_origin;
	// noob::mat4 stage.stage.view_mat;
	// noob::mat4 stage.projection_mat;

	logger::log("[Application] done init.");
	user_init();
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
			// init();

			try
			{
				// TODO: Replace with AngelScript
				// chai->eval_file(p.generic_string());
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


void noob::application::draw()
{
	stage.projection_mat = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 1.0f, 400.0f);

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
