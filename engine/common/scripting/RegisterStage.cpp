#include "RegisterStage.hpp"


void noob::register_stage(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("stage", sizeof(noob::stage), asOBJ_VALUE); assert(r >= 0 );
	r = script_engine->RegisterObjectBehaviour("stage", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_stage_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("stage", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_stage_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void init()", asMETHOD(noob::stage, init), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void tear_down()", asMETHOD(noob::stage, tear_down), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void update(double)", asMETHOD(noob::stage, update), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void draw(float, float)", asMETHOD(noob::stage, draw), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "body_handle body(body_type, const shape_handle, float, const vec3& in, const versor& in, bool)", asMETHOD(noob::stage, body), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("stage", "void actor(const body_handle, const animated_model_handle, const shader_handle)", asMETHODPR(noob::stage, actor, (const noob::bodies_holder::handle, const noob::animated_models_holder::handle, const noob::shaders_holder::handle), void), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("stage", "void actor(const body_handle, const scaled_model, const shader_handle)", asMETHODPR(noob::stage, actor, (const noob::bodies_holder::handle, const noob::scaled_model, const noob::shaders_holder::handle), void), asCALL_THISCALL); assert( r >= 0 );
	// r = script_engine->RegisterObjectMethod("stage", "void actor(const shape_handle, float, const vec3& in, const versor& in, const scaled_model, const shader_handle)", asMETHODPR(noob::stage, actor, (const noob::bodies_holder::handle, float, const noob::vec3&, const noob::versor&, const noob::scaled_model, const noob::shaders_holder::handle), void), asCALL_THISCALL); assert( r >= 0 );
	// void actor(const noob::shapes_holder::handle, float mass, const noob::vec3& pos, const noob::versor& orient, const noob::shaders_holder::handle)
	r = script_engine->RegisterObjectMethod("stage", "void actor(const shape_handle, float, const vec3& in, const versor& in, const shader_handle, const reflectance_handle)", asMETHODPR(noob::stage, actor, (const noob::shapes_holder::handle, float mass, const noob::vec3&, const noob::versor&, const noob::shaders_holder::handle, const reflectances_holder::handle), void), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("stage", "void scenery(const basic_mesh& in, const vec3& in, const versor& in, const shader_handle, const reflectance_handle, const string& in)", asMETHOD(noob::stage, scenery), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "void set_light(uint, const light_handle)", asMETHOD(noob::stage, set_light), asCALL_THISCALL); assert(r >= 0);
	// r = script_engine->RegisterObjectMethod("stage", "light_handle get_light()", asMETHOD(noob::stage, get_light), asCALL_THISCALL); assert(r >= 0);
	
	r = script_engine->RegisterObjectMethod("stage", "void write_graph(const string& in)", asMETHOD(noob::stage, write_graph), asCALL_THISCALL); assert(r >= 0);
	
	r = script_engine->RegisterObjectProperty("stage", "bool show_origin", asOFFSET(noob::stage, show_origin)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("stage", "mat4 view_mat", asOFFSET(noob::stage, view_mat)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("stage", "mat4 projection_mat", asOFFSET(noob::stage, projection_mat)); assert(r >= 0);
	r = script_engine->RegisterObjectProperty("stage", "vec4 ambient_light", asOFFSET(noob::stage, ambient_light)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("stage", "vec3 eye_pos", asOFFSET(noob::stage, eye_pos)); assert(r >= 0);
}
