#include "RegisterGlobals.hpp"

void noob::register_globals(asIScriptEngine* script_engine)
{
	noob::globals& g = noob::globals::get_instance();

	int r;
	r = script_engine->RegisterGlobalFunction("shape_handle sphere_shape(float)", asMETHOD(noob::globals, sphere_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shape_handle box_shape(float, float, float)", asMETHOD(noob::globals, box_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shape_handle cylinder_shape(float, float)", asMETHOD(noob::globals, cylinder_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shape_handle cone_shape(float, float)", asMETHOD(noob::globals, cone_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shape_handle hull_shape(const vector_vec3& in, const string& in)", asMETHOD(noob::globals, hull_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shape_handle static_trimesh_shape(const basic_mesh& in, const string& in)", asMETHOD(noob::globals, static_trimesh_shape), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	// r = script_engine->RegisterGlobalFunction("mesh_handle add_mesh(const basic_mesh& in)", asMETHOD(noob::globals, add_mesh), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	// r = script_engine->RegisterGlobalFunction("model_handle model_by_shape(const shape_handle)", asMETHOD(noob::globals, model_by_shape), asCALL_THISCALL_ASGLOBAL, &g); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("scaled_model sphere_model(float)", asMETHOD(noob::globals, sphere_model), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("scaled_model box_model(float, float, float)", asMETHOD(noob::globals, box_model), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("scaled_model cylinder_model(float, float)", asMETHOD(noob::globals, cylinder_model), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("scaled_model cone_model(float, float)", asMETHOD(noob::globals, cone_model), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("scaled_model model_from_mesh(const basic_mesh& in)", asMETHOD(noob::globals, model_from_mesh), asCALL_THISCALL_ASGLOBAL, &g); assert(r >= 0);
	r = script_engine->RegisterGlobalFunction("animated_model_handle animated_model(const string& in)", asMETHOD(noob::globals, animated_model), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	// r = script_engine->RegisterGlobalFunction("skeleton_handle skeleton(const string& in)", asMETHOD(noob::globals, skeleton), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("light_handle set_light(const light& in, const string& in)", asMETHOD(noob::globals, set_light), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("light_handle get_light(const string& in)", asMETHOD(noob::globals, set_light), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("reflectance_handle set_reflectance(const reflectance& in, const string& in)", asMETHOD(noob::globals, set_reflectance), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("reflectance_handle get_reflectance(const string& in)", asMETHOD(noob::globals, get_reflectance), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_shader(const basic_uniform& in, const string& in)", asMETHODPR(noob::globals, set_shader, (const noob::basic_renderer::uniform&, const std::string&), void), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("void set_shader(const triplanar_gradmap_uniform& in, const string& in)", asMETHODPR(noob::globals, set_shader, (const noob::triplanar_gradient_map_renderer::uniform&, const std::string&), void), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("shader_handle get_shader(const string& in)", asMETHOD(noob::globals, get_shader), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );

	r = script_engine->RegisterGlobalFunction("void set_actor_blueprints(const actor_blueprints& in, const string& in)", asMETHOD(noob::globals, set_actor_blueprints), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
	r = script_engine->RegisterGlobalFunction("actor_blueprints_handle get_actor_blueprints(const string& in)", asMETHOD(noob::globals, get_actor_blueprints), asCALL_THISCALL_ASGLOBAL, &g); assert( r >= 0 );
}
