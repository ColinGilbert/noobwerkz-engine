#include "RegisterControls.hpp"

void noob::register_controls(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterObjectType("controls", sizeof(noob::controls), asOBJ_VALUE); assert(r >= 0 );
	
	r = script_engine->RegisterObjectBehaviour("controls", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_controls_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("controls", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_controls_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	
	r = script_engine->RegisterObjectMethod("controls", "void set_eye_pos(const vec3& in)", asMETHOD(noob::controls, set_eye_pos), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "void set_eye_target(const vec3& in)", asMETHOD(noob::controls, set_eye_target), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "void set_eye_up(const vec3& in)", asMETHOD(noob::controls, set_eye_up), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("controls", "void increment_eye_pos(const vec3& in)", asMETHOD(noob::controls, increment_eye_pos), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "void increment_eye_target(const vec3& in)", asMETHOD(noob::controls, increment_eye_target), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "void increment_eye_up(const vec3& in)", asMETHOD(noob::controls, increment_eye_up), asCALL_THISCALL); assert( r >= 0 );

	r = script_engine->RegisterObjectMethod("controls", "vec3 get_eye_pos() const", asMETHOD(noob::controls, get_eye_pos), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "vec3 get_eye_target() const", asMETHOD(noob::controls, get_eye_target), asCALL_THISCALL); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("controls", "vec3 get_eye_up() const", asMETHOD(noob::controls, get_eye_up), asCALL_THISCALL); assert( r >= 0 );
}
