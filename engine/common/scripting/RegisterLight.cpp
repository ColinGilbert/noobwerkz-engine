#include "RegisterLight.hpp"

void noob::register_light(asIScriptEngine* script_engine)
{
	int r;
	r = script_engine->RegisterEnum("light_type"); assert(r >= 0);
	r = script_engine->RegisterEnumValue("light_type", "DIRECTIONAL", 0); assert(r >= 0);
	r = script_engine->RegisterEnumValue("light_type", "POINT", 1); assert(r >= 0);
	r = script_engine->RegisterEnumValue("light_type", "SPOT", 2); assert(r >= 0);

	r = script_engine->RegisterObjectType("light", sizeof(noob::light), asOBJ_VALUE | asOBJ_POD | asGetTypeTraits<noob::light>() | asOBJ_APP_CLASS_ALLINTS); assert(r >= 0);
	r = script_engine->RegisterObjectBehaviour("light", asBEHAVE_CONSTRUCT,  "void f()", asFUNCTION(as_light_constructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectBehaviour("light", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(as_light_destructor_wrapper), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = script_engine->RegisterObjectMethod("light", "void set_position(vec3& in)", asMETHOD(noob::light, set_position), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "void set_colour(const vec3& in)", asMETHOD(noob::light, set_colour), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "void set_radius(float)", asMETHOD(noob::light, set_radius), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "void set_falloff(float r)", asMETHOD(noob::light, set_falloff), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "vec3 get_position()", asMETHOD(noob::light, get_position), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "vec3 get_colour()", asMETHOD(noob::light, get_colour), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "float get_radius()", asMETHOD(noob::light, get_radius), asCALL_THISCALL); assert( r >= 0 );	
	r = script_engine->RegisterObjectMethod("light", "float get_falloff()", asMETHOD(noob::light, get_falloff), asCALL_THISCALL); assert( r >= 0 );	
}
