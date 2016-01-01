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
	ww << "[AngelScript] " /*Message: Section = " << msg->section << " row = " << msg->row << " col: " << msg->col << ", type: " */ << message_type << ": " << msg->message;
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

	r = script_engine->RegisterObjectType("vec2", sizeof(vec2), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for vec2

	r = script_engine->RegisterObjectType("vec3", sizeof(vec3), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for vec3
	// vec3();
	// create from 3 scalars
	// vec3(float x, float y, float z);
	// create from vec2 and a scalar
	// vec3(const vec2& vv, float z);
	// create from truncated vec4
	// vec3(const vec4& vv);
	// vec3(const vec3& vv);
	// vec3(const btVector3&);

	// template <class Archive>
	// void serialize( Archive & ar )
	// {
	// 	ar(v);
	// }

	// add vector to vector
	// vec3 operator+(const vec3& rhs) const;
	// add scalar to vector
	// vec3 operator+(float rhs) const;
	// because user's expect this too
	// vec3& operator+=(const vec3& rhs);
	// subtract vector from vector
	// vec3 operator-(const vec3& rhs) const;
	// add vector to vector
	// vec3 operator-(float rhs) const;
	// because users expect this too
	// vec3& operator-=(const vec3& rhs);
	// multiply with scalar
	// vec3 operator*(float rhs) const;
	// because users expect this too
	// vec3& operator*=(float rhs);
	// divide vector by scalar
	// vec3 operator/(float rhs) const;
	// because users expect this too
	// vec3& operator=(const vec3& rhs);

	// internal data
	// std::array<float,3> v;

	// float& operator[](int x)
	// {
	// return v[x];
	// }

	//std::string to_string() const
	// {
	// fmt::MemoryWriter w;
	// w << "(" << v[0] << ", " << v[1] << ", " << v[2] << ")";
	// return w.str();
	// }




	r = script_engine->RegisterObjectType("vec4", sizeof(vec4), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for vec4

	r = script_engine->RegisterObjectType("versor", sizeof(versor), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for versor

	r = script_engine->RegisterObjectType("mat3", sizeof(mat3), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for mat3

	r = script_engine->RegisterObjectType("mat4", sizeof(mat4), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );
	// TODO: Constructors, operators, for mat4

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
	// TODO(?)
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
	// r = script_engine->RegisterGlobalFunction("versor slerp(const versor& in, const versor& in)", asFUNCTIONPR(slerp, (const versor&, const versor&), versor), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor normalize(const versor& in)", asFUNCTIONPR(normalize, (const versor&), versor), asCALL_CDECL); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("versor slerp(const versor& in, const versor& in, float)", asFUNCTION(slerp), asCALL_CDECL); assert(r >= 0); //PR(slerp, (const versor&, const versor&, float), versor), asCALL_CDECL); assert(r >= 0);


	r = script_engine->RegisterObjectType("basic_mesh", 0, asOBJ_REF); assert(r >= 0 );
	r = script_engine->RegisterObjectType("stage", 0, asOBJ_REF); assert(r >= 0 );

	// TODO: Constructors, data members, functions for basic_mesh
	// std::vector<noob::vec3> basic_mesh::vertices;
	// std::vector<noob::vec3> basic_mesh::normals;
	// std::vector<noob::vec3> basic_mesh::texcoords;
	// std::vector<uint32_t> basic_mesh::indices;

	// script_engine->RegisterGlobalFunction("basic_mesh sphere(float, float, size_t)", asFUNCTION(mesh_utils::sphere), asCALL_CDECL); assert(r >= 0);
	// script_engine->RegisterGlobalFunction("basic_mesh cone(float, float, size_t)", asFUNCTION(mesh_utils::cone), asCALL_CDECL); assert(r >= 0);
	// script_engine->RegisterGlobalFunction("basic_mesh box(float, float, float)", asFUNCTION(mesh_utils::cylinder), asCALL_CDECL); assert(r >= 0);
	// script_engine->RegisterGlobalFunction("basic_mesh hull(const std::vector<noob::vec3>& in)", asFUNCTION(mesh_utils::hull), asCALL_CDECL); assert(r >= 0);
	// script_engine->RegisterGlobalFunction("basic_mesh cone(float, float, size_t)", asFUNCTION(mesh_utils::cone), asCALL_CDECL); assert(r >= 0);
	// basic_mesh cube(float width, float height, float depth, size_t subdivides = 0);
	// basic_mesh cylinder(float radius, float height, size_t segments = 0);
	// basic_mesh swept_sphere(float radius, size_t x_segment = 0, size_t y_segment = 0);
	// basic_mesh catmull_sphere(float radius);
	// basic_mesh hull(const std::vector<noob::vec3>& points);
	// basic_mesh csg(const noob::basic_mesh& a, const noob::basic_mesh& b, const noob::csg_op op);


	// r = script_engine->RegisterObjectType("animated_model_handle", sizeof(noob::stage.animated_models::handle), asOBJ_VALUE | asOBJ_POD); assert(r >= 0 );

	// void stage.init();
	// void stage.tear_down();
	// void stage.update(double dt);
	// void stage.draw(float window_width, float window_height) const;
	// noob::bodies_holder::handle stage.body(noob::body_type, noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0), bool ccd = false);
	// noob::shapes_holder::handle stage.sphere(float r);
	// noob::shapes_holder::handle stage.box(float x, float y, float z);
	// noob::shapes_holder::handle stage.cylinder(float r, float h);
	// noob::shapes_holder::handle stage.cone(float r, float h);
	// noob::shapes_holder::handle stage.hull(const std::vector<noob::vec3>&);
	// noob::shapes_holder::handle stage.static_trimesh(const noob::meshes_holder::handle);
	// noob::meshes_holder::handle stage.add_mesh(const noob::basic_mesh& h);
	// noob::basic_models_holder::handle stage.basic_model(const noob::meshes_holder::handle);
	// noob::animated_models_holder::handle stage.animated_model(const std::string& filename);
	// noob::skeletal_anims_holder::handle stage.skeleton(const std::string& filename);
	// void stage.set_light(const noob::light&, const std::string&);
	// noob::light stage.get_light(const std::string&);
	// void stage.set_reflection(const noob::reflection&, const std::string&);
	// noob::reflection stage.get_reflection(const std::string&);			
	// void stage.set_shader(const noob::prepared_shaders::info&, const std::string& name);
	// noob::shaders_holder::handle stage.get_shader(const std::string& name);
	// noob::component_tag stage.scales_tag;
	// noob::component_tag stage.mesh_tag;
	// noob::component_tag stage.path_tag;
	// noob::component_tag stage.shape_tag;
	// noob::component_tag stage.shape_type_tag;
	// noob::component_tag tage.body_tag;
	// noob::component_tag stage.basic_model_tag;
	// noob::component_tag stage.animated_model_tag;
	// noob::component_tag stage.skeletal_anim_tag;
	// noob::component_tag stage.shader_tag;
	// noob::meshes_holder stage.meshes;
	// noob::basic_models_holder stage.basic_models;
	// noob::animated_models_holder stage.animated_models;
	// noob::shapes_holder stage.shapes;
	// noob::bodies_holder stage.bodies;
	// noob::skeletal_anims_holder stage.skeletal_anims;
	// noob::lights_holder stage.lights;
	// noob::reflections_holder stage.reflections;
	// noob::shaders_holder stage.shaders;
	// const noob::shapes_holder::handle stage.unit_sphere_shape;
	// const noob::shapes_holder::handleunit_cube_shape;
	// const noob::shapes_holder::handleunit_capsule_shape;
	// const noob::shapes_holder::handleunit_cylinder_shape;
	// const noob::shapes_holder::handleunit_cone_shape;
	// const noob::meshes_holder::handle unit_sphere_mesh;
	// const noob::meshes_holder::handle unit_cube_mesh;
	// const noob::meshes_holder::handle unit_capsule_mesh;
	// const noob::meshes_holder::handle unit_cylinder_mesh;
	// const noob::meshes_holder::handle unit_cone_mesh;
	// const noob::basic_models_holder::handle unit_sphere_model;
	// const noob::basic_models_holder::handle unit_cube_model;
	// const noob::basic_models_holder::handle unit_capsule_model;
	// const noob::basic_models_holder::handle unit_cylinder_model;
	// const noob::basic_models_holder::handle unit_cone_model;
	// const noob::shaders_holder::handle debug_shader;
	// const noob::shaders_holder::handle default_triplanar_shader;
	// const noob::shaders_holder::handle uv_shader;
	// es::entity stage.actor(float radius, float height, const noob::animated_models_holder::handle, const std::string& shading);
	// es::entity stage.prop(const noob::bodies_holder::handle, const std::string& shading);
	// es::entity stage.prop(const noob::bodies_holder::handle, const noob::basic_models_holder::handle, const std::string& shading);
	// es::entity stage.scenery(const noob::meshes_holder::handle, const noob::vec3& pos, const std::string& shading, const noob::versor& orient = noob::versor(0.0, 0.0, 0.0, 1.0) );
	// es::storage stage.pool;
	// noob::basic_mesh stage.make_mesh(const noob::shapes_holder::handle);
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
	stage.projection_mat = noob::perspective(60.0f, static_cast<float>(window_width)/static_cast<float>(window_height), 0.1f, 500.0f);

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
