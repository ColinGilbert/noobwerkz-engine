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
	assert(script_engine == 0); // TODO: Look into a little.
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
	ww << "[AngelScript] " /*Message: Section = " << msg->section << " row = " << msg->row << " col: " << msg->col << ", type: " */ << message_type << ": " << msg->message;
	logger::log(ww.str());
}


void noob::application::init()
{
	ui_enabled = true;
	gui.init(*prefix, window_width, window_height);
	voxels.init();
	stage.init();

	// Used by ANgelScript to capture the result of the last registering.
	int r;
	//r = script_engine->RegisterGlobalFunction("", asFUNCTION(func_name), asCALL_CDECL);
	script_engine->SetMessageCallback(asFUNCTION(angel_message_callback), 0, asCALL_CDECL);

	r = script_engine->RegisterObjectType("vec2", sizeof(vec2), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for vec2

	r = script_engine->RegisterObjectType("vec3", sizeof(vec3), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for vec3

	r = script_engine->RegisterObjectType("vec4", sizeof(vec4), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for vec4

	r = script_engine->RegisterObjectType("versor", sizeof(versor), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for versor

	r = script_engine->RegisterObjectType("mat3", sizeof(mat3), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for mat3

	r = script_engine->RegisterObjectType("mat4", sizeof(mat4), asOBJ_VALUE | asOBJ_POD); assert( r >= 0 );
	// TODO: Constructors, operators, for mat4

	// TODO: The host of mathematical functions defined in MathFuncs.hpp

	// float length(const vec3& v);
	// float length2(const vec3& v);
	// vec3 normalize(const vec3& v);
	// float dot(const vec3& a, const vec3& b);
	// vec3 cross(const vec3& a, const vec3& b);
	// float get_squared_dist(vec3 from, vec3 to);
	// float direction_to_heading(vec3 d);
	// vec3 heading_to_direction(float degrees);

	// bool linearly_dependent(const noob::vec3& a, const noob::vec3& b, const noob::vec3& c);

	// matrix functions
	// mat3 zero_mat3();
	// mat3 identity_mat3();
	// mat4 zero_mat4();
	// mat4 identity_mat4();
	// float determinant(const mat4& mm);
	// mat4 inverse(const mat4& mm);
	// mat4 transpose(const mat4& mm);

	// mat4 translate(const mat4& m, const vec3& v);
	// mat4 rotate_x_deg(const mat4& m, float deg);
	// mat4 rotate_y_deg(const mat4& m, float deg);
	// mat4 rotate_z_deg(const mat4& m, float deg);
	// mat4 scale(const mat4& m, const vec3& v);

	// vec3 translation_from_mat4(const mat4& m);
	// vec3 scale_from_mat4(const mat4& m);

	// mat4 look_at(const vec3& cam_pos, vec3 targ_pos, const vec3& up);
	// mat4 perspective(float fovy, float aspect, float near, float far);
	// mat4 ortho(float left, float right, float bottom, float top, float near, float far);

	// versor quat_from_axis_rad(float radians, float x, float y, float z);
	// versor quat_from_axis_deg(float degrees, float x, float y, float z);
	// versor quat_from_mat4(const mat4&);

	// mat4 quat_to_mat4(const versor& q);
	// float dot(const versor& q, const versor& r);
	// versor slerp(const versor& q, const versor& r);
	// void create_versor(float* q, float a, float x, float y, float z);
	// void mult_quat_quat(float* result, float* r, float* s);
	// void normalize_quat(float* q);
	// void quat_to_mat4(float* m, float* q); 
	// versor normalize(const versor& q);
	// void print(const versor& q);
	// versor slerp(versor& q, versor& r, float t);

	r = script_engine->RegisterObjectType("basic_mesh", 0, asOBJ_REF); assert( r >= 0 );

	// TODO: Constructors, functionsfor basic_mesh
	// std::vector<noob::vec3> basic_mesh::vertices;
	// std::vector<noob::vec3> basic_mesh::normals;
	// std::vector<noob::vec3> basic_mesh::texcoords;
	// std::vector<uint32_t> basic_mesh::indices;

	// static noob::basic_mesh cone(float radius, float height, size_t segments = 0);
	// static noob::basic_mesh cube(float width, float height, float depth, size_t subdivides = 0);
	// static noob::basic_mesh cylinder(float radius, float height, size_t segments = 0);
	// static noob::basic_mesh swept_sphere(float radius, size_t x_segment = 0, size_t y_segment = 0);
	// static noob::basic_mesh catmull_sphere(float radius);
	// static noob::basic_mesh hull(const std::vector<noob::vec3>& points);
	// static noob::basic_mesh csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op);

	// TODO: Register component-related stuff. For each template specialization. Blargh.

	// class component
	// class component::handle;
	// T* get(component<T>::handle h);
	// component<T>::handle add(const T& t);
	// bool exists(component<T>::handle h);
	// void set_name(component<T>::handle h, const std::string& name);
	// component<T>::handle get_handle(const std::string& name);
	// bool name_exists(const std::string& name);

	// TODO: Register stage-related stuff
	//
	// typedef noob::component<std::unique_ptr<noob::basic_model>> basic_model_component;	
	// typedef noob::component<std::unique_ptr<noob::animated_model>> animated_model_component;
	// typedef noob::component<std::unique_ptr<noob::skeletal_anim>> skeleton_component;
	// typedef noob::component<noob::light> light_component;
	// typedef noob::component<noob::reflection> reflection_component;
	// typedef noob::component<noob::prepared_shaders::info> shader_component;
	// typedef noob::component<std::unique_ptr<noob::shape>> shape_component;
	// typedef noob::component<noob::body> body_component;
	// typedef noob::component<noob::prop> prop_component;	
	// typedef noob::component<noob::actor> actor_component;
	// typedef noob::component<noob::scenery> scenery_component;

	// basic_model_component::handle basic_model(const noob::basic_mesh&);	
	// Loads a serialized model (from cereal binary)

	animated_model_component::handle animated_model(const std::string& filename);
	skeleton_component::handle skeleton(const std::string& filename);
	actor_component::handle actor(const basic_model_component::handle, const skeleton_component::handle, const noob::vec3&, const noob::versor& v = noob::versor(0.0, 0.0, 0.0, 1.0));
	prop_component::handle prop(const basic_model_component::handle, const noob::vec3&, const noob::versor&);
	scenery_component::handle scenery(const basic_model_component::handle, const noob::vec3&, const noob::versor&);
	light_component::handle light(const noob::light&);
	reflection_component::handle reflection(const noob::reflection&);
	shader_component::handle shader(const noob::prepared_shaders::info&, const std::string& name);

	// These cache the shape for reuse, as they are simple parametrics.
	noob::shape_component::handle sphere(float r);
	noob::shape_component::handle box(float x, float y, float z);
	noob::shape_component::handle cylinder(float r, float h);
	noob::shape_component::handle cone(float r, float h);
	noob::shape_component::handle capsule(float r, float h);
	noob::shape_component::handle plane(const noob::vec3& normal, float offset);

	noob::shape_component::handle hull(const std::vector<noob::vec3>& points, const std::string& name);
	noob::shape_component::handle trimesh(const noob::basic_mesh& mesh, const std::string& name);

	// stage,scenery_component sceneries;
	// stage,light_component lights;
	// stage.reflection_component reflections;
	// stage.shader_component shaders;
	// stage.shape_component shapes;
	// stage.body_component bodies;
	// stage.basic_model_component basic_models;
	// stage.animated_model_component animated_models;
	// stage.skeleton_component skeletons;

	bool paused;

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


	// TODO: Replace with AngelScript
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

	noob::mat4 proj = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 2000.0f);

	bgfx::setViewTransform(0, &view_mat.m[0], &proj.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);

	stage.draw();

	// gui.text("The goat stumbled upon the cheese", 150.0, 50.0);
}


void noob::application::accept_ndof_data(const noob::ndof::data& info)
{
	if (info.movement == true)
	{
		// logger::log(fmt::format("[Sandbox] NDOF data: T({0}, {1}, {2}) R({3}, {4}, {5})", info.translation[0], info.translation[1], info.translation[2], info.rotation[0], info.rotation[1], info.rotation[2]));
		float damping = 360.0;
		noob::vec3 rotation(info.rotation);
		noob::vec3 translation(info.translation);
		view_mat = noob::rotate_x_deg(view_mat, -rotation[0]/damping);
		view_mat = noob::rotate_y_deg(view_mat, -rotation[1]/damping);
		view_mat = noob::rotate_z_deg(view_mat, -rotation[2]/damping);
		view_mat = noob::translate(view_mat, noob::vec3(-translation[0]/damping, -translation[1]/damping, -translation[2]/damping));
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
