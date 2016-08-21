#include "RegisterStage.hpp"

#include "Actor.hpp"

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
	r = script_engine->RegisterObjectMethod("stage", "ghost_handle ghost(const shape_handle, const vec3& in, const versor& in)", asMETHOD(noob::stage, ghost), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("stage", "joint_handle joint(const body_handle, const vec3& in, const body_handle, const vec3& in)", asMETHOD(noob::stage, joint), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("stage", "actor_handle actor(const actor_blueprints_handle, uint32, const vec3& in, const versor& in)", asMETHOD(noob::stage, actor), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("stage", "scenery_handle scenery(const shape_handle, shader_handle, reflectance_handle, const vec3& in, const versor& in)", asMETHOD(noob::stage, scenery), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "particle_system_handle add_particle_system(const particle_system_descriptor& in)", asMETHOD(noob::stage, add_particle_system), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "particle_system_descriptor get_particle_system_properties(const particle_system_handle) const", asMETHOD(noob::stage, get_particle_system_properties), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "void set_particle_system_properties(const particle_system_handle, const particle_system_descriptor& in)", asMETHOD(noob::stage, set_particle_system_properties), asCALL_THISCALL); assert(r >= 0);

	// void activate_particle_system(const noob::particle_system_handle, bool) noexcept(true);
	r = script_engine->RegisterObjectMethod("stage", "void activate_particle_system(const particle_system_handle, bool)", asMETHOD(noob::stage, activate_particle_system), asCALL_THISCALL); assert(r >= 0);
	r = script_engine->RegisterObjectMethod("stage", "void set_light(uint32, const light_handle)", asMETHOD(noob::stage, set_light), asCALL_THISCALL); assert(r >= 0);
	// r = script_engine->RegisterObjectMethod("stage", "light_handle get_light()", asMETHOD(noob::stage, get_light), asCALL_THISCALL); assert(r >= 0);

	// r = script_engine->RegisterObjectMethod("stage", "void write_graph(const string& in)", asMETHOD(noob::stage, write_graph), asCALL_THISCALL); assert(r >= 0);

	r = script_engine->RegisterObjectProperty("stage", "bool show_origin", asOFFSET(noob::stage, show_origin)); assert(r >= 0);	
	r = script_engine->RegisterObjectProperty("stage", "vec4 ambient_light", asOFFSET(noob::stage, ambient_light)); assert(r >= 0);	
}
